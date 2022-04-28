#ifndef WOLF_TOKEN_HPP
#define WOLF_TOKEN_HPP

#include <cstddef>

#define DeclToken(NAME, VAL) w##NAME = VAL,
enum class wTokenType {
    #include "Token.decl"
};

struct wToken;

template<typename T>
struct Node;

using wNode = Node<wToken>;

struct wToken {
    wTokenType type = wTokenType::wNone;
    
    size_t line   = 0;
    size_t column = 0;

    union {
        wNode* Node;
        char* ID;
        char Letter;
        char* str;
        int Indent;
    } value;

    wToken(size_t line = 0, size_t column = 0, wTokenType type = wTokenType::wNone) 
        : line(line), column(column), type(type) {}

    const char* GetTypeName() const {
    switch (type) {
    case wTokenType::wPercent:
        return "'\%\%'";

    case wTokenType::wAsterisk:
        return "'*'";

    case wTokenType::wPlus:
        return "'+'";

    case wTokenType::wComma:
        return "','";

    case wTokenType::wColon:
        return "':'";

    case wTokenType::wSemicolon:
        return "';'";

    case wTokenType::wQuestion:
        return "'?'";

    case wTokenType::wSqBracketL:
        return "'['";

    case wTokenType::wSqBracketR:
        return "']'";

    case wTokenType::wVertLine:
        return "'|'";

    case wTokenType::wTilde:
        return "'~'";

    case wTokenType::wBlock1N:
        return "[ ]+";

    case wTokenType::wBlock0N:
        return "[ ]*";

    case wTokenType::wBlock01:
        return "[ ]~";

    case wTokenType::wBlockQ:
        return "[ ]?";

    case wTokenType::wBlockQ1N:
        return "[ ]?+";

    case wTokenType::wBlockQ0N:
        return "[ ]?*";

    case wTokenType::wBlockQ01:
        return "[ ]?~";
    default:
        break;
    }

    #define DeclToken(NAME, VAL)    \
        case wTokenType::w##NAME:   \
            return #NAME;           \
    
    switch (type) {
        #include "Token.decl"
    
    default:
        return "Undefined";
        break;
    }
    }

    void Print() const {
        std::cout << "TOKEN(" << line << ":" << column << ") : " << GetTypeName() << "\n";
    }
};

#endif // WOLF_TOKEN_HPP