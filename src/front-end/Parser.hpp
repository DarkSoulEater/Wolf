#ifndef WOLF_PARSER_HPP
#define WOLF_PARSER_HPP

#include <iostream>
#include "Logger.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Tree.hpp"
#include "Generator.hpp"

#pragma GCC diagnostic ignored "-Wswitch-enum"

template<typename Token>
class wParser {
private:
    wLexer<Token> lexer_;

    const wState& lexer_state_;

public:
    wParser(const char* file_name) : lexer_(file_name), lexer_state_(lexer_.State()) {}
    ~wParser() {}

#define DeclLexerTarget(NAME)                       \
    wToken Get##NAME() {                            \
        wToken ret(0, 0, wTokenType::w##NAME);      \
        wToken token = lexer_.GetToken();           \
                                                    \
        if (token.type != wTokenType::w##NAME){     \
            Error(ret, token, "");                 }\
        return token;                               \
    }

    DeclLexerTarget(ID);
    DeclLexerTarget(Letter);
    DeclLexerTarget(Colon);
    DeclLexerTarget(Semicolon);
    DeclLexerTarget(VertLine);
    DeclLexerTarget(EOF);
    DeclLexerTarget(Keyword)
    DeclLexerTarget(Code)
    DeclLexerTarget(Percent)
    DeclLexerTarget(Asterisk)
    DeclLexerTarget(Plus)
    DeclLexerTarget(Comma)
    DeclLexerTarget(Question)
    DeclLexerTarget(SqBracketL)
    DeclLexerTarget(SqBracketR)
    DeclLexerTarget(Tilde)

    #define InitRule(RULE_NAME)                         \
        wToken ret_val(0, 0, wTokenType::w##RULE_NAME); \
        ret_val.value.Node = new Node(ret_val);         \
        wToken token = lexer_.ViewToken();

    #define Success() return ret_val; 

    #define Get(TYPE) ((token = Get##TYPE()).type == wTokenType::w##TYPE)
    #define View(TYPE) ((token = lexer_.ViewToken()).type == wTokenType::w##TYPE)
    #define error(TEXT) Error(ret_val, token, TEXT)

    #define SaveLocation() ;

    #define ValNode ret_val.value.Node
    #define RetVal ret_val.value
    #define TokenVal token.value
    #define TokenNode token.value.Node

    #define InsertToken() ret_val.value.Node->Insert(token)
    #define InsertNode() ret_val.value.Node->Insert(token.value.Node)
    #define InsertKids() ret_val.value.Node->InsertKid(token.value.Node)

    /*wToken Get() {
        wToken ret(0, 0, wTokenType::);
        wToken token = lexer_.ViewToken();

        switch (token.type) {
        case wTokenType::: {
            
        } break;
        
        default:
            Error(ret);
            return ret;
        }

        return ret;
    }*/

    wToken GetDepend() { // <Depend> : [[<ID> | <Letter> | '[' <Depend> ']'   ['+' | '*' | '~' | '?' ['+' | '*' | '~']~]?~]? [<Code>]~]+ ['|' <Depend>]*
        InitRule(Depend);

        if (!View(ID) && !View(Letter) && !View(SqBracketL)) // [ ]+
            error("expected <ID> or <Letter> or '[' before %s");

        while (View(ID) || View(Letter) || View(SqBracketL)) { // [ ]*
            switch (token.type) {
            case wTokenType::wID: { // <ID>
                Get(ID);
                InsertToken();
            } break;

            case wTokenType::wLetter: { // <Letter>
                Get(Letter);
                InsertToken();
            } break;

            case wTokenType::wSqBracketL: { // '['
                Get(SqBracketL); // '['
                Get(Depend); // <Depend>
                wNode* save_depend = token.value.Node;
                InsertNode();
                Get(SqBracketR); // ']'

                token = lexer_.ViewToken();
                switch (token.type) {
                case wTokenType::wPlus: { // '+'
                    Get(Plus);
                    save_depend->data_.type = wTokenType::wBlock1N;
                } break;

                case wTokenType::wAsterisk: { // '*'
                    Get(Asterisk);
                    save_depend->data_.type = wTokenType::wBlock0N;
                } break;

                case wTokenType::wTilde: { // '~'
                    Get(Tilde);
                    save_depend->data_.type = wTokenType::wBlock01;
                } break;

                case wTokenType::wQuestion: { // '?'
                    Get(Question);

                    token = lexer_.ViewToken();
                    switch (token.type) {
                    case wTokenType::wPlus: { // '?+'
                        Get(Plus);
                        save_depend->data_.type = wTokenType::wBlockQ1N;
                    } break;

                    case wTokenType::wAsterisk: { // '?*'
                        Get(Asterisk);
                        save_depend->data_.type = wTokenType::wBlockQ0N;
                    } break;

                    case wTokenType::wTilde: { // '?~'
                        Get(Tilde);
                        save_depend->data_.type = wTokenType::wBlockQ01;
                    } break;
                    
                    default:
                        save_depend->data_.type = wTokenType::wBlockQ;
                        break;
                    }
                } break;

                default: // '?'
                    ret_val.value.Node->kids_.PopBack();
                    ret_val.value.Node->InsertKid(save_depend);
                    delete save_depend;
                    break;
                }

            } break;
            
            default:
                break;
            }

            if (View(Code)) { // <Code>~
                Get(Code);
                InsertNode();
            }
        }
        
        // ['|' <Depend>]*
        while (View(VertLine)) { // [ ]*
            Get(VertLine); // '|'
            InsertToken();
            
            Get(Depend); // <Depend>
            InsertKids();
        }

        Success();
    }

    wToken GetRule() { // <Rule> : <ID> ':' <Depend>
        InitRule(Rule);

        Get(ID); // <ID> 
        InsertToken();

        Get(Colon); // ':'

        Get(Depend); // <Depend>
        InsertKids();

        Get(Semicolon); // ';'

        Success();
    }

    wToken GetDeclToken() {
        InitRule(DeclToken);

        Success();
    }

    wToken GetUnion() {
        InitRule(Union);

        Success();
    }

    wToken GetDeclarations() { // [<Keyword> [<Code> | <ID>]?]*
        InitRule(Declarations);

        while (View(Keyword)) { // [ ]*
            Get(Keyword); // <Keyword>
            InsertToken();

            if (View(Code)) { // <Code>
                Get(Code);

                static int cnt = 0;
                if (cnt++) {
                    error("redefinition union");
                } else {
                    InsertNode();
                }
            } else { // <ID>
                Get(ID);
                InsertToken();
            }
        }

        Success();
    }

    wToken GetPreamble() {
        InitRule(Preamble);

        Success();
    }


    wToken GetG() { // <G> : [<Preamble>]~ <Declarations> '%%' [<Rule>]+ '%%'
        InitRule(G);

        Get(Declarations); // <Declarations>
        InsertNode();

        Get(Percent); // '%%'

        // [<Rule>]+
        if (!View(ID)) // <Rule>+
            error("expected <ID> before %s");

        while (View(ID)) { // <Rule>*
            Get(Rule);
            InsertNode();
        }

        if (!View(Percent))
            error("expected <Rule> or '%%%' before %s");
        else 
            Get(Percent); // '%%'

        Get(EOF); // <EOF>

        Success();
    }

    wToken StartParce() {
        //for (wToken tk = lexer_.GetToken(); tk.type != wTokenType::wEOF; tk = lexer_.GetToken()) {
        //    tk.Print();
        //}
        wToken tk = GetG();
        //tk.Print();
        std::cout << "Parse OK\n";
        tk.value.Node->GraphicsDump();
        std::cout << "Graph OK\n";
        //Generator gen(tk.value.Node);
        //gen.Generate();
        return tk;
        //if (tk.type != wTokenType::wFail) GetG().value.Node->Print();
        Token t;
        return t;
    }

private:
    wToken Error(wToken& ret, wToken& token, const char* text) {
        if (text[0] == '\0')
            wLogger.PrintError("test/simple_grammar.txt", token.line, token.column, "expected %s, but find %s", ret.GetTypeName(), token.GetTypeName()); // TODO: Add file name
        else if (ret.type == wTokenType::wKeyword) {
            wLogger.LogError("test/simple_grammar.txt", token.line, token.column, text); // TODO: Add file name
        } else 
            wLogger.PrintError("test/simple_grammar.txt", token.line, token.column, text, token.GetTypeName()); // TODO: Add file name
    }
};

#endif // WOLF_PARSER_HPP