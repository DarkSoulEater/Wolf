#include <iostream>
#include "Token.hpp"
#include "Parser.hpp"
#include <string.h>

void ParseArgs(const int argc, const char* argv[]);

#include <Vector.hpp>
#include "HashTable.hpp"
#include "HashTableKeyword.hpp"
#include "Windows.h"
#include "Logger.hpp"

struct Key {
    char key_[32];

    bool operator==(const Key& key) {
        return strcmp(key.key_, key_) == 0;
    }
};

int main0();

int main(const int argc, const char* argv[]) {
    /*HANDLE hConsole = GetStdHa ndle(STD_OUTPUT_HANDLE);
    SetConsoleMode(hConsole, ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);

    ParseArgs(argc, argv);
    //char *str = "src/main.cpp:84:10: \u001B[31mError:\u001B[0m 'd' was not declared in this scope\n";
    for (size_t i = 0 ; i < 100; ++i) {
        printf("src/main.cpp:84:10: \u001B[%dmError: Hello world 15123 123 123\u001B[0m 'd' was not declared in this scope %d\n", i, i);
    }
    return 0;
    std::cout << "OK\n";
    std::cerr << "ERR\n";
    printf("\x1B[31mTexting\033[0m\t\t\n");

    for(int k = 1; k < 10; k++)
    {
    // pick the colorattribute k you want
    SetConsoleTextAttribute(hConsole, k);
    std::cout << k << " I want to be nice today!" << std::endl;
  }
    
    char a;
    std::cin >> a;*/

    wParser<wToken> gen("test/simple_grammar.txt");
    gen.StartParce();
    //int val = gen.ParseFile("test/simple_grammar.txt");
    //std::cout << val;
    //std::cin >> a;
    return 0;
}

void ParseArgs(const int argc, const char* argv[]) {
    for (int i = 0; i < argc; ++i) {
        printf("%s ", argv[i]);
    }
    printf("\n\n");
}