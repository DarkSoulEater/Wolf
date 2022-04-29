#include "Logger.hpp"
#include "Buffer.hpp"
#include "Token.hpp"
#include "Parser.hpp"
#include "Generator.hpp"
#include "Global.hpp"

void ParseArgs(const int argc, const char* argv[]);

int main(const int argc, const char* argv[]) {
    ParseArgs(argc, argv);

    if (InputFileName[0] == '\0') {
        wLogger.LogError("No input filename");
        return 0;
    }

    Buffer buffer(InputFileName);

    wParser<wToken> parser(buffer.GetData());
    auto token = parser.StartParce();
    
    if (token.type != wTokenType::wFail) {
        
    }
    return 0;
}

void ParseArgs(const int argc, const char* argv[]) {
    for (int i = 0; i < argc; ++i) {
        wLogger.Log(argv[i]);
    }
    //printf("\n\n");
}