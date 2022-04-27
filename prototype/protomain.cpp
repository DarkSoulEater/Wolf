#include <iostream>
#include "Buffer.hpp"

int protomain() {
    const char* input_data_file_name = "test/simple_grammar.txt";
    Buffer buf(input_data_file_name);
    char* data = (char*) buf.GetData();
    while (*data) {
        std::cout << *data++;
    }
}
/*
enum class TokenType {
    wNone,
    wLexerError,
    wFail,

    wSym64 = 'A',

    Rule,
    ID,
};

struct Token {
    TokenType type;

    size_t line;
    size_t column;

    union value {
        //wNode* node;
        char* text;
        char* ID;
    };
};

Token GetID() {
    Token token = GetToken();
    if (token.type == TokenType::ID) return Token;
    token.type = TokenType::wFail;
    return token;
}

Token GetRule() {
    // Init;
    Token tk[3];

    tk[1] = GetID();
    tk[2] = GetRule();
    if (tk[1].type != TokenType::wFail
     && tk[2].type != TokenType::wFail) {
        tk[0]->type = TokenType::Rule;

        {
            // User code
            //tree.Insert(tk[0]->value.node, tk[1]);
        }
        
        return tk[0];
    }

    Rallback();

    tk[1] = GetID();
    if (tk[1].type != TokenType::wFail) {
        tk[0]->type = TokenType::Rule;

        {
            User code
        }

        return tk[0];
    }
}*/