#ifndef WOLF_TOKEN_HPP
#define WOLF_TOKEN_HPP

#include <cstddef>

enum class wTokenType {
    wNone       = 0,
    wLexerError = 1,
    wFail       = 2,
    wEOF        = 3,
    wKeyword    = 4,

    wID         = 10,
    wLetter     = 11,
    wCode       = 12,
    wLine       = 13,
    wIndent     = 14,
    wColon      = 58,  // ':'
    wSemicolon  = 59,  // ';'
    wVertLine   = 124, // '|'

    wUnion,
    wDeclarations,

    wRule,
    wRules,
    wDepend,
    wDepends,
    wProgram,
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
    case wTokenType::wRule:
        return "Rule";
        break;

    case wTokenType::wRules:
        return "Rules";
        break;

    case wTokenType::wDepend:
        return "Depend";
        break;

    case wTokenType::wDepends:
        return "Depends";
        break;

    case wTokenType::wID:
        return "ID";
        break;

    case wTokenType::wLetter:
        return "Letter";
        break;

    case wTokenType::wFail:
        return "Fail";
        break;

    case wTokenType::wEOF:
        return "EOF";
        break;

    case wTokenType::wLexerError:
        return "LexerError";
        break;

    case wTokenType::wColon:
        return "Colon";
        break;

    case wTokenType::wSemicolon:
        return "Semicolon";
        break;
    
    case wTokenType::wVertLine:
        return "VertLine";
        break;

    case wTokenType::wCode:
        return "Code";
        break;

    case wTokenType::wKeyword:
        return "Keyword";
        break;

    case wTokenType::wUnion:
        return "Union";
        break;

    case wTokenType::wLine:
        return "Line";
        break;

    case wTokenType::wIndent:
        return "Indent";
        break;
    
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