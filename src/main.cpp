#include <iostream>
#include "Token.hpp"
#include "Parser.hpp"

void ParseArgs(const int argc, const char* argv[]);

#include <Vector.hpp>


int main(const int argc, const char* argv[]) {
    ParseArgs(argc, argv);
    //std::cout << typeid(COUNTER).name() << " " << __COUNTER__ << "\n";
    Parser<int, Token> gen;
    int val = gen.ParseFile("test/simple_grammar.txt");
    std::cout << val;

    return 0;
}

void ParseArgs(const int argc, const char* argv[]) {
    for (int i = 0; i < argc; ++i) {
        printf("%s ", argv[i]);
    }
    printf("\n\n");
}