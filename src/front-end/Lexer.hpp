#ifndef WOLF_LEXER_HPP
#define WOLF_LEXER_HPP

#include <cctype>
#include "Buffer.hpp"
#include "Vector.hpp"
#include "Token.hpp"

template <typename Token>
class Lexer {
private:
    Buffer buffer_;
    Vector<Token> tokens_;

    size_t state_;
    size_t line_;
    size_t column_;

    char* data_;
public:
    Lexer() : state_(0), line_(1), column_(0), data_(nullptr) {}
    ~Lexer() {}

    bool OpenFile(const char* file_name) {
        bool f = buffer_.OpenFile(file_name);
        data_ = (char*) buffer_.GetData();
        return f;
    }

    #define ReturnToken(TYPE)           \
        token.type = TokenType::TYPE;   \
        tokens_.PushBack(token);        \
        return tokens_[state_++];     

    Token GetToken() {
        if (state_ < tokens_.Size()) return tokens_[state_++];

        char c = 0;
        while (std::isspace(c = Getc())) 
            ;

        Token token(line_, column_, TokenType::EndOfFile);

        switch (c) {
        case '<':
            token.ID = data_;
            while (std::isalpha(c = Getc()))
                ;
            
            if (c == '>') {
                *(data_ - 1) = '\0';
                ReturnToken(ID);
            }
        
            ReturnToken(Error);
            break;

        case '\'':
            c = Getc();
            token.Letter = c;
            c = Getc();
            if (c == '\'') {
                ReturnToken(Letter);
            }
            ReturnToken(Error);
            break;

        case '{':
            token.Code = data_;
            while ((c = Getc()) != '}' && c != '\0')
                ;
            if (c == '}') {
                *(data_ - 1) = '\0';
                ReturnToken(Code_);
            }
            ReturnToken(Error);
            break;

        case ':':
            ReturnToken(Colon);
            break;

        case '|':
            ReturnToken(VertLine);
            break;

        case ';':
            ReturnToken(Semicolon);
            break;

        case '\0':
            ReturnToken(EndOfFile);
            break;
        
        default:
            ReturnToken(Error);
            break;
        }
    }
    #undef ReturnToken

    inline const size_t& GetState() const { return state_; }

    void Rollback(size_t state) {
        state_ = state;
    }

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