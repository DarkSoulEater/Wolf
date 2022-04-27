#ifndef WOLF_PARSER_HPP
#define WOLF_PARSER_HPP

#include <iostream>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Tree.hpp"
#include "Generator.hpp"

template<typename Token>
class wParser {
private:
    wLexer<Token> lexer_;

    const wState& lexer_state_;

public:
    wParser(const char* file_name) : lexer_(file_name), lexer_state_(lexer_.State()) {}
    ~wParser() {}

#define DeclLexerTarget(NAME)                   \
    Token Get##NAME() {                         \
        Token token = lexer_.GetToken();        \
        if (token.type != wTokenType::w##NAME)  \
            token.type = wTokenType::wFail;     \
        token.Print();                          \
        return token;                           \
    }

    DeclLexerTarget(ID);
    DeclLexerTarget(Letter);
    DeclLexerTarget(Colon);
    DeclLexerTarget(Semicolon);
    DeclLexerTarget(VertLine);
    DeclLexerTarget(EOF);
    DeclLexerTarget(Keyword)

    wToken GetCode() {
        // Initial
        wState state = lexer_.State();
        wToken tk[1] = {};

        if ((tk[0] = lexer_.GetToken()).type == wTokenType::wCode) {
            return tk[0];
        } else {
            lexer_.StateRollback(state);
            tk[0].type = wTokenType::wNone;
            return tk[0];
        }
    }

    wToken GetDepend() { // TODO:Clear
        // Initial
        wState state = lexer_.State();
        wToken tk[3] = {};

        // Branch 1
        if ((tk[1] = GetID()).type == wTokenType::wID
         && (tk[2] = GetDepend()).type == wTokenType::wDepend) {
            tk[0].type = wTokenType::wDepend;

            tk[0].value.Node = new Node(tk[0]);
            tk[0].value.Node->Insert(tk[1]);
            tk[0].value.Node->InsertKid(tk[2].value.Node);

            delete tk[2].value.Node;

            return tk[0];
        }

        lexer_.StateRollback(state);

        // Branch 2
        if ((tk[1] = GetLetter()).type == wTokenType::wLetter
         && (tk[2] = GetDepend()).type == wTokenType::wDepend) {
            tk[0].type = wTokenType::wDepend;

            tk[0].value.Node = new Node(tk[0]);
            tk[0].value.Node->Insert(tk[1]);
            tk[0].value.Node->InsertKid(tk[2].value.Node);

            delete tk[2].value.Node;

            return tk[0];
        }

        lexer_.StateRollback(state);

        // Branch 3
        if ((tk[1] = GetID()).type == wTokenType::wID) {
            tk[0].type = wTokenType::wDepend;

            tk[0].value.Node = new Node(tk[0]);
            tk[0].value.Node->Insert(tk[1]);

            if ((tk[2] = GetCode()).type == wTokenType::wCode) {
                tk[0].value.Node->Insert(tk[2].value.Node);
            }

            return tk[0];
        }

        lexer_.StateRollback(state);

        // Branch 4
        if ((tk[1] = GetLetter()).type == wTokenType::wLetter) {
            tk[0].type = wTokenType::wDepend;

            tk[0].value.Node = new Node(tk[0]);
            tk[0].value.Node->Insert(tk[1]);

            if ((tk[2] = GetCode()).type == wTokenType::wCode) {
                tk[0].value.Node->Insert(tk[2].value.Node);
            }

            return tk[0];
        }

        // Fail
        lexer_.StateRollback(state);
        tk[0].type = wTokenType::wFail;
        return tk[0];
    }

    wToken GetDepends() {
        // Initial
        wState state = lexer_.State();
        wToken tk[4] = {};

        // Branch 1
        if ((tk[1] = GetDepend()).type == wTokenType::wDepend
         && (tk[2] = GetVertLine()).type == wTokenType::wVertLine
         && (tk[3] = GetDepends()).type == wTokenType::wDepends) {
            tk[0].type = wTokenType::wDepends;

            tk[0].value.Node = new Node(tk[0]);
            tk[0].value.Node->Insert(tk[1].value.Node);
            tk[0].value.Node->InsertKid(tk[3].value.Node);

            delete tk[3].value.Node;

            return tk[0];
        }

        // Branch 2
        lexer_.StateRollback(state);

        if ((tk[1] = GetDepend()).type == wTokenType::wDepend) {
            tk[0].type = wTokenType::wDepends;

            tk[0].value.Node = new Node(tk[0]);
            tk[0].value.Node->Insert(tk[1].value.Node);

            return tk[0];
        }

        // Fail
        lexer_.StateRollback(state);
        tk[0].type = wTokenType::wFail;
        return tk[0];
    }

    wToken GetRule() {
        // Initial
        wState state = lexer_.State();
        wToken tk[5] = {};

        // Branch 1
        if ((tk[1] = GetID()).type == wTokenType::wID
         && (tk[2] = GetColon()).type == wTokenType::wColon
         && (tk[3] = GetDepends()).type == wTokenType::wDepends
         && (tk[4] = GetSemicolon()).type == wTokenType::wSemicolon) {
            tk[0].type = wTokenType::wRule;

            tk[0].value.Node = new Node(tk[0]);
            tk[0].value.Node->Insert(tk[1]);
            //tk[0].value.Node->Insert(tk[3].value.Node);
            tk[0].value.Node->InsertKid(tk[3].value.Node);

            return tk[0];
        }

        // Fail
        lexer_.StateRollback(state);
        tk[0].type = wTokenType::wFail;
        return tk[0];
    }

    wToken GetRules() {
        // Initial
        wState state = lexer_.State();
        wToken tk[3] = {};

        // Branch 1
        if ((tk[1] = GetRule()).type == wTokenType::wRule
         && (tk[2] = GetRules()).type == wTokenType::wRules) {
            tk[0].type = wTokenType::wRules;

            tk[0].value.Node = new Node(tk[0]);
            tk[0].value.Node->Insert(tk[1].value.Node);
            tk[0].value.Node->InsertKid(tk[2].value.Node);

            delete tk[2].value.Node;

            return tk[0];
        }

        lexer_.StateRollback(state);

        // Branch 2
        if ((tk[1] = GetRule()).type == wTokenType::wRule) {
            tk[0].type = wTokenType::wRules;

            tk[0].value.Node = new Node(tk[0]);
            tk[0].value.Node->Insert(tk[1].value.Node);

            return tk[0];
        }

        // Fail
        lexer_.StateRollback(state);
        tk[0].type = wTokenType::wFail;
        return tk[0];
    }

    wToken GetDeclarations() {
        // Initial
        wState state = lexer_.State();
        wToken tk[3] = {};

        // Branch 1
        if ((tk[1] = GetKeyword()).type == wTokenType::wKeyword
         && (tk[2] = GetCode()).type == wTokenType::wCode) {
            tk[0].type = wTokenType::wUnion;

            tk[0].value.Node = new Node(tk[0]);
            tk[0].value.Node->Insert(tk[2].value.Node);

            return tk[0];    
        }

        // Fail
        lexer_.StateRollback(state);
        tk[0].type = wTokenType::wFail;
        return tk[0];
    }

    wToken GetG() {
        // Initial
        wState state = lexer_.State();
        wToken tk[4] = {};

        // Branch 1
        if ((tk[1] = GetDeclarations()).type == wTokenType::wUnion
         && (tk[2] = GetRules()).type == wTokenType::wRules
         && (tk[3] = GetEOF()).type == wTokenType::wEOF) {
            tk[0].type = wTokenType::wProgram;

            tk[0].value.Node = new Node(tk[0]);
            tk[0].value.Node->Insert(tk[1].value.Node);
            tk[0].value.Node->Insert(tk[2].value.Node);

            tk[0].value.Node->GraphicsDump();

            return tk[0];
        
        }

        lexer_.StateRollback(state);

        // Branch 2
        if ((tk[1] = GetRules()).type == wTokenType::wRules
         && (tk[2] = GetEOF()).type == wTokenType::wEOF) {
            tk[0].type = wTokenType::wProgram;

            tk[0].value.Node = new Node(tk[0]);
            tk[0].value.Node->Insert(tk[1].value.Node);

            std::cerr << "OK\n";
            tk[0].value.Node->GraphicsDump();

            return tk[0];
        }

        // Fail
        std::cout << "Fail\n";
        lexer_.StateRollback(state);
        tk[0].type = wTokenType::wFail;
        return tk[0];
    }

    wToken StartParce() {
        //for (wToken tk = lexer_.GetToken(); tk.type != wTokenType::wEOF; tk = lexer_.GetToken()) {
        //    tk.Print();
        //}
        wToken tk = GetG();
        Generator gen(tk.value.Node);
        gen.Generate();
        return tk;
        //if (tk.type != wTokenType::wFail) GetG().value.Node->Print();
        return tk;
    }
};

#endif // WOLF_PARSER_HPP