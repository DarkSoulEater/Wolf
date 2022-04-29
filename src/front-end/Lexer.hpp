#ifndef WOLF_LEXER_HPP
#define WOLF_LEXER_HPP

#include <cctype>

#include "Logger.hpp"
#include "Tree.hpp"
#include "Token.hpp"
#include "Type.hpp"
#include "Keyword.hpp"
#include "HashMap.hpp"
#include "Global.hpp"

typedef size_t wState;

template <typename Token>
class wLexer {
private:
    Vector<wToken> tokens_;

    wState active_token_;

private:
    HashMap<MapString32, wKeyword> keyword_table_;
public:
    wLexer(void* data) : active_token_(0), line_(1), column_(0), data_((char*)data) {
        #define DeclKeyword(NAME) keyword_table_.Insert(#NAME, wKeyword::w##NAME);
        #include "Keyword.decl"
    }

    ~wLexer() {}

    Token GetToken() {
        assert(active_token_ <= tokens_.Size());
        if (active_token_ == tokens_.Size()) tokens_.PushBack(Lex());
        return tokens_[active_token_++];
    }

    Token ViewToken() {
        assert(active_token_ <= tokens_.Size());
        if (active_token_ == tokens_.Size()) tokens_.PushBack(Lex());
        return tokens_[active_token_];
    }

    void UngetToken() {
        assert(active_token_);
        --active_token_;
    }

    #define ReturnToken(TYPE)           \
        token.type = wTokenType::TYPE;  \
        return token;     

    Token Lex() {
        assert(active_token_ < tokens_.Size());

        char c = 0;
        while (std::isspace(c = Getc())) 
            ;

        wToken token(line_, column_, wTokenType::wEOF);

        switch (c) {
        case '/': {
            if (*data_ == '/') { //
                while ((c = Getc()) != '\n' && c != '\0')
                    ;
            } else if (*data_ == '*') {
                size_t line = line_;
                size_t column = column_;

                c = Getc();
                c = Getc();

                while (c = Getc()) {
                    if (c == '/' && *(data_ - 2) == '*') break;
                }

                if (c == '\0') {
                    wLogger.PrintError(InputFileName, line, column + 1, "expected end of comment /* */");
                    ReturnToken(wLexerError);
                }

            } else {
                wLogger.PrintError(InputFileName, line_, column_, "expected comment");
                ReturnToken(wLexerError);
            }
            return Lex();
        } break;

        case '<': {
            token.value.ID = data_;

            if (!isalpha(c = Getc()) && c != '_') {
                if (c == '>') wLogger.PrintError(InputFileName, line_, column_, "expected identifier in \"<>\" (example <auf>)");
                else wLogger.PrintError(InputFileName, line_, column_, "incorrect identifier ([a-Z,_][a-Z,0-9,_]*)");
                ReturnToken(wLexerError);
            }
            
            size_t id_len = 1;

            while (std::isalpha(c = Getc()) || c == '_' || isdigit(c)) {
                if (++id_len == 32) {
                    *(data_ - 1) = '\0';
                    wLogger.LogWarning(InputFileName, line_, column_, "identifier too long (must be < 32). The rest will be eaten by wolves");
                }
            }
            
            if (c == '>') {
                *(data_ - 1) = '\0';
                ReturnToken(wID);
            }
            
            wLogger.PrintError(InputFileName, line_, column_, "expected '>' befor '%c'", c);
            ReturnToken(wLexerError);
        } break;

        case '\'':
            c = Getc();
            if (c == '\n') {
                wLogger.PrintError(InputFileName, line_, column_, "'\\n' in valid palce");
                ReturnToken(wLexerError);
            }

            if (c == '\'') {
                wLogger.PrintError(InputFileName, line_, column_, "expected letter in ' '");
                ReturnToken(wLexerError);
            }

            if (c == '\\') {
                c = Getc();
                switch (c) {
                case '\\':
                case '\'':
                case '"':
                // case 'n':
                    break;
                
                default:
                    wLogger.PrintError(InputFileName, line_, column_, "uncorrect escape sequences in ' '");
                    ReturnToken(wLexerError);
                    break;
                }
            }

            token.value.Letter = c;
            // if (c == 'n') token.value.Letter = '\n';
            c = Getc();
            if (c == '\'') {
                ReturnToken(wLetter);
            }
            wLogger.PrintError(InputFileName, line_, column_, "expected '\'' at end of token");
            ReturnToken(wLexerError);
            break;

        case '{': {
            int cnt_opened = 1;
            size_t line = line_;
            size_t column = column_;

            token.type = wTokenType::wCode;
            token.value.Node = new wNode(token);

            while (cnt_opened != 0) {
                c = Getc();
                
                // If EOF
                if (c == '\0') {
                    delete token.value.Node;
                    wLogger.PrintError(InputFileName, line, column, "expected '}' at end of input");
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
            if (*data_ == '%') {
                c = Getc();
                ReturnToken(wPercent);
            }
            
            static char buf[33];
            MapString32 str;
            int i = 0;

            while (isalpha(*data_)) {
                c = Getc();

                if (i < 33) {
                    str[i++] = c;
                } else {
                    wLogger.LogWarning(InputFileName, line_, column_, "too long word, all characters after 32 will be ignored");
                }
            }

            auto res = keyword_table_.Find(str);
            if (res != nullptr) {
                wLogger.Log(InputFileName, line_, column_, "Is key word! Auuuuuuu!!!");
                token.value.Keyword = *res;
                ReturnToken(wKeyword);
            } else {
                wLogger.LogError(InputFileName, line_, column_, "Not key word! Wuuuuuu(((");
                ReturnToken(wLexerError);
            }
        } break;

        case '*':
            ReturnToken(wAsterisk);
            break;

        case '+':
            ReturnToken(wPlus);
            break;

        case ',':
            ReturnToken(wComma);
            break;

        case ':':
            ReturnToken(wColon);
            break;

        case ';':
            ReturnToken(wSemicolon);
            break;

        case '?':
            ReturnToken(wQuestion);
            break;

        case '[':
            ReturnToken(wSqBracketL);
            break;

        case ']':
            ReturnToken(wSqBracketR);
            break;

        case '|':
            ReturnToken(wVertLine);
            break;

        case '~':
            ReturnToken(wTilde);
            break;

        case '\0':
            ReturnToken(wEOF);
            break;
        
        default:
            wLogger.PrintError(InputFileName, line_, column_, "\'%c\' an unknown character in this scope", c);
            ReturnToken(wLexerError);
            break;
        }
    }
    #undef ReturnToken

private: 
    size_t line_;
    size_t column_;

    char* data_;

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