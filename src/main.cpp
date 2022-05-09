#include "Logger.hpp"
#include "Buffer.hpp"
#include "Token.hpp"
#include "Parser.hpp"
#include "Generator.hpp"
#include "Global.hpp"
#include <set>

#include "Set.hpp"

void ParseArgs(const int argc, const char* argv[]);

int main(const int argc, const char* argv[]) {
    ParseArgs(argc, argv);

    if (InputFileName == nullptr) {
        wLogger.LogError("no input file");
        return 0;
    }

    Buffer buffer(InputFileName);

    wParser<wToken> parser(buffer.GetData());
    auto token = parser.StartParce();
    
    if (token.type != wTokenType::wFail) {
        wGenerator generator(token.value.Node, parser);
        generator.Generate("C:/Users/eleno/C++/Wolf/wolf_parser/Parser.cpp");
    } else printf("ERR");

    return 0;
}

void ParseArgs(const int argc, const char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        wLogger.Log(argv[i]);
        InputFileName = argv[i];
    }
    
    if (InputFileName == nullptr) InputFileName = "C:/Users/eleno/C++/Wolf/test/simple_grammar.txt";
}