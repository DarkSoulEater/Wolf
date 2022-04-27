#ifndef WOLF_LEXER_HPP
#define WOLF_LEXER_HPP

#include <cctype>
#include "Tree.hpp"
#include "Buffer.hpp"
#include "Vector.hpp"
#include "Token.hpp"

typedef size_t wState;

template <typename Token>
class wLexer {
private:
    Buffer buffer_;
    Vector<wToken> tokens_;

    wState active_token_;

    size_t line_;
    size_t column_;

    char* data_;
public:
    wLexer(const char* file_name) : buffer_(file_name), active_token_(0), line_(1), column_(0) {
        data_ = (char*) buffer_.GetData();
    }

    ~wLexer() {}

    inline wState State() const { return active_token_; }

    void StateRollback(wState state) {
        active_token_ = state;
    }

    #define ReturnToken(TYPE)           \
        token.type = wTokenType::TYPE;  \
        tokens_.PushBack(token);        \
        return tokens_[active_token_++];     

    Token GetToken() {
        if (active_token_ < tokens_.Size()) return tokens_[active_token_++];

        char c = 0;
        while (std::isspace(c = Getc())) 
            ;

        wToken token(line_, column_, wTokenType::wEOF);

        switch (c) {
        case '<':
            token.value.ID = data_;
            while (std::isalpha(c = Getc()))
                ;
            
            if (c == '>') {
                *(data_ - 1) = '\0';
                ReturnToken(wID);
            }
        
            ReturnToken(wLexerError);
            break;

        case '\'':
            c = Getc();
            token.value.Letter = c;
            c = Getc();
            if (c == '\'') {
                ReturnToken(wLetter);
            }
            ReturnToken(wLexerError);
            break;

        case '{': {
            int cnt_opened = 1;
            token.type = wTokenType::wCode;
            token.value.Node = new wNode(token);

            while (cnt_opened != 0) {
                c = Getc();
                
                // If EOF
                if (c == '\0') {
                    delete token.value.Node;
                    ReturnToken(wLexerError);
                }

                // Skip space
                if (isspace(c)) continue;
                
                // Check code end
                if (c == '}') {
                    if (cnt_opened == 1) {
                        *(data_ - 1) = '\0';    
                        break;
                    }
                }

                // Add new line
                wNode* line_node = new wNode(wToken(line_, column_, wTokenType::wLine));
                line_node->data_.value.str = data_ - 1;

                wNode* indent = new wNode(wToken(line_, column_, wTokenType::wIndent));
                indent->data_.value.Indent = cnt_opened - (c == '}' ? 1 : 0);
                line_node->Insert(indent);

                token.value.Node->Insert(line_node);

                while (c != '\n' && c != '\0') {
                    if (c == '}') --cnt_opened;
                    else if (c == '{') ++cnt_opened;

                    if (cnt_opened == 0) {
                        *(data_ - 1) = '\0';
                        goto Break;
                    }

                    c = Getc();
                }
                *(data_ - 1) = *(data_ - 2) = '\0';
            }
            Break:{}
            
            ReturnToken(wCode);
        } break;

        case '%': {
            char* ptr = data_;  
            while (isalpha(*data_))
                c = Getc();
            
            ReturnToken(wKeyword);
            
            ReturnToken(wLexerError);
        } break;

        case ':':
            ReturnToken(wColon);
            break;

        case '|':
            ReturnToken(wVertLine);
            break;

        case ';':
            ReturnToken(wSemicolon);
            break;

        case '\0':
            ReturnToken(wEOF);
            break;
        
        default:
            ReturnToken(wLexerError);
            break;
        }
    }
    #undef ReturnToken

private:
    char Getc() {
        char*& c = data_;
        if (*c == '\0') return *c;
        if (*c == '\r') ++c;

        ++column_;
        if (*c == '\n') {
            column_ = 0;
            line_  += 1;
        }

        return *c++;
    }
};

#endif // WOLF_LEXER_HPP