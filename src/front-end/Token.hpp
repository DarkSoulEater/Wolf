#ifndef WOLF_TOKEN_HPP
#define WOLF_TOKEN_HPP

#include <cstddef>
#include <iostream>

#define TOKEN(TOKEN_NAME, LEVEL) TOKEN_NAME,
enum class TokenType {
    #include "Token.def"
};

struct Token {
    Token(size_t line = 0, size_t column = 0, TokenType type = TokenType::Error) : line(line), column(column), type(type) {}

    TokenType type;
    size_t line;
    size_t column;
    
    union {
        char* ID;
        char* Code;
        char Letter;
    };

    void Print() {
        std::cerr << "TOKEN(" << line << ":" << column << "): ";

        #define TOKEN(TOKEN_NAME, LEVEL)                  \
            case TokenType::TOKEN_NAME: {          \
                std::cerr << #TOKEN_NAME << "\n";  \
        } break;                                    

        switch (type) {
        #include "Token.def"
        
        default:
            std::cerr << "Error: Undefined token\n";
            abort();
            break;
        }
    }

    const char* GetTypeName() {
        #define TOKEN(TOKEN_NAME, LEVEL)           \
            case TokenType::TOKEN_NAME: {          \
                return #TOKEN_NAME;                \
        } break;       
        
        switch (type) {
        #include "Token.def"
        
        default:
            std::cerr << "Error: Undefined token\n";
            abort();
            break;
        }
    }
};

#endif // WOLF_TOKEN_HPP