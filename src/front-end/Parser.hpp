#ifndef WOLF_PARSER_HPP
#define WOLF_PARSER_HPP

#include <iostream>
#include "Logger.hpp"
#include "Lexer.hpp"
#include "Tree.hpp"
#include "HashMap.hpp"
#include "Type.hpp"
#include "Global.hpp"

#pragma GCC diagnostic ignored "-Wswitch-enum"

template<typename Token>
class wParser {
    friend class wGenerator;
private:
    wLexer<Token> lexer_;

public:
    wParser(void* data) : lexer_(data) {}
    ~wParser() {}

#define DeclLexerTarget(NAME)                       \
    wToken Get##NAME() {                            \
        wToken ret(0, 0, wTokenType::w##NAME);      \
        wToken token = lexer_.GetToken();           \
                                                    \
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

    #define Get(TYPE)                       \
        token = Get##TYPE();                \
        //ret_val.next.Combining(token.next); \
                                          
    //((token = Get##TYPE()).type == wTokenType::w##TYPE)

    #define View(TYPE) ((token = lexer_.ViewToken()).type == wTokenType::w##TYPE)
    #define error(TEXT) Error(ret_val, token, TEXT)

    #define SaveLocation() {                                \
        ret_val.value.Node->data_.line = token.line;        \
        ret_val.value.Node->data_.column = token.column;    }

    #define ValNode ret_val.value.Node
    #define RetVal ret_val.value
    #define TokenVal token.value
    #define TokenNode token.value.Node

    #define InsertToken() ret_val.value.Node->Insert(token)
    #define InsertNode() ret_val.value.Node->Insert(token.value.Node)
    #define InsertKids() ret_val.value.Node->InsertKid(token.value.Node)

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
                save_depend->data_.line = token.line;
                save_depend->data_.column = token.column;

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
        SaveLocation();
        InsertToken();

        Get(Colon); // ':'

        Get(Depend); // <Depend>
        InsertKids();

        Get(Semicolon); // ';'

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
        wToken token = GetG();

        if (syntax_state_) {
            token.value.Node->GraphicsDump();

            SemanticAnalysis(token.value.Node);

            token.value.Node->GraphicsDump();

            if (semantic_state_) return token;
            
            delete token.value.Node;
            return wToken(wTokenType::wFail);
        }

        delete token.value.Node;
        return wToken(wTokenType::wFail);
    }

private:
    bool syntax_state_ = true;

    wToken Error(wToken& ret, wToken& token, const char* text) {
        if (text[0] == '\0')
            wLogger.PrintError(InputFileName, token.line, token.column, "expected %s, but find %s", ret.GetTypeName(), token.GetTypeName());
        else if (ret.type == wTokenType::wKeyword) {
            wLogger.LogError(InputFileName, token.line, token.column, text);
        } else 
            wLogger.PrintError(InputFileName, token.line, token.column, text, token.GetTypeName());

        syntax_state_ = false;
    }

    HashMap<MapString32, int> map_;

private:
    bool semantic_state_ = true;

    HashMap<MapString32, wNode*> lexer_token_;
    HashMap<MapString32, wNode*> target_token_;
    HashMap<MapString32, wNode*> rule_token_;

    const char* start_rule_name;

    void SemanticAnalysis(wNode* node) {
        size_t i = 0;

        if (node->kids_[i]->data_.type == wTokenType::wDeclarations) {
            wNode* kid = node->kids_[i];

            for (size_t j = 0; j < kid->kids_.Size(); j += 2) {
                if (kid->kids_[j]->data_.value.Keyword == wKeyword::wtoken) {
                    auto res = lexer_token_.Insert(MapString32(kid->kids_[j + 1]->data_.value.ID), kid->kids_[j + 1]);
                    if (!res) {
                        wLogger.LogWarning(InputFileName, kid->kids_[j]->data_.line, kid->kids_[j]->data_.column, "redefenition token, why?");
                    }
                } else {
                    // NOINT
                }
            }

            ++i;
        }

        for (; i < node->kids_.Size(); ++i) {
            wNode* kid = node->kids_[i];

            if (kid->data_.type == wTokenType::wRule) {
                auto res = rule_token_.Insert(kid->kids_[0]->data_.value.ID, kid);
                if (!res){
                    auto find_res = rule_token_.Find(kid->kids_[0]->data_.value.ID);
                    if (find_res == nullptr) assert(0 && "Map table error!");

                    wNode* rule = *find_res;
                    kid->kids_[0]->data_.type = wTokenType::wVertLine;
                    rule->InsertKid(kid);
                    kid->data_.type = wTokenType::wNone;

                    // Warning
                    wLogger.PrintWarning(InputFileName, kid->kids_[0]->data_.line, kid->kids_[0]->data_.column, 
                                        "redefinition rule <%s> (%llu:%llu), rules will be merged into one flock",
                                        (*find_res)->data_.value.ID, 
                                        (*find_res)->data_.line, 
                                        (*find_res)->data_.column);
                }

                for (size_t j = 1; j < kid->kids_.Size(); ++j) {
                    AnalysisRule(kid->kids_[j], kid);
                }
            }
        }

#ifdef WOLF_DB_PRINT_SEMANTIC_TOKEN
        wLogger.Log("Lexer token:");
        for (auto u : lexer_token_) {
            std::cout << "    " << u.first.str << "\n";
        }

        std::cout << "\n";

        wLogger.Log("Rule token:");
        for (auto u : rule_token_) {
            std::cout << "    " << u.second->data_.value.ID << "\n";
        }

        std::cout << "\n";

        wLogger.Log("Targer token:");
        for (auto u : target_token_) {
            std::cout << "    " << u.second->data_.value.ID << "\n";
        }
        std::cout << "\n";
#endif // WOLF_DB_PRINT_SEMANTIC_TOKEN

        if (StartRule) {
            auto res = rule_token_.Find(StartRule);
            if (res) {
                target_token_.Insert(StartRule, *res);
                start_rule_name = StartRule;
            } else {
                wLogger.PrintWarning("there is no rule <%s>, the first wolf is chosen as the main wolf");

                for (size_t i = 0; i < node->kids_.Size(); ++i) {
                    if (node->kids_[i]->data_.type == wTokenType::wRule) {
                        target_token_.Insert(node->kids_[i]->kids_[0]->data_.value.ID, node->kids_[i]);
                        start_rule_name = node->kids_[i]->kids_[0]->data_.value.ID;
                        break;
                    }
                }
            }
        } else {
            for (size_t i = 0; i < node->kids_.Size(); ++i) {
                if (node->kids_[i]->data_.type == wTokenType::wRule) {
                    target_token_.Insert(node->kids_[i]->kids_[0]->data_.value.ID, node->kids_[i]);
                    start_rule_name = node->kids_[i]->kids_[0]->data_.value.ID;
                    break;
                }
            }
        }

        for (auto& trg : target_token_) {
            auto res_rule = rule_token_.Find(trg.first);
            auto res_lexer = lexer_token_.Find(trg.first);
            if (!res_rule && !res_lexer) {
                wNode* target = trg.second;
                semantic_state_ = false;
                wLogger.PrintError(InputFileName, target->data_.line, target->data_.column, "there is no rule generating <%s>, lone wolf", target->data_.value.ID);
            }
        }

        for (i = 0; i < node->kids_.Size(); ++i) {
            wNode* kid = node->kids_[i];
            if (kid->data_.type == wTokenType::wRule) {
                auto res = target_token_.Find(kid->kids_[0]->data_.value.ID);
                if (!res && FlagSkipUselessRule) {
                    kid->data_.type = wTokenType::wNone;

                    wLogger.PrintWarning(InputFileName, kid->data_.line, kid->data_.column, "<%s> is a useless rule, dead wolf", kid->kids_[0]->data_.value.ID);
                } else {
                    Set<MapString32> general;
                    StartLL1Check(general, node->kids_[i]);
                }
            }
        }

#ifdef WOLF_DB_LOG_RULE_FIRST_TERMINAL
        for (size_t i = 0; i < node->kids_.Size(); ++i) {
            if (node->kids_[i]->data_.type == wTokenType::wRule) {
                wLogger.PrintDebug("rule: <%s>", node->kids_[i]->data_.value.ID);

                for (auto u : node->kids_[i]->info_->next) {
                    std::cout << "    " << u.str << "\n";
                }
            }
        }
#endif

        for (size_t j = 0; j < node->kids_.Size(); ++j) {
            if (node->kids_[j]->data_.type == wTokenType::wDeclarations) {
                wNode* decl = node->kids_[j];
                
                for (auto& u: lexer_token_) {
                    if (u.first.str[0] == 'S' && u.first.str[1] == 'y' && u.first.str[2] == 'm') {
                        decl->Insert(wTokenType::wKeyword);
                        wNode* id = new wNode;
                        id->data_.type = wTokenType::wID;
                        id->data_.value.ID = u.first.str;
                        decl->Insert(id);
                    }
                }
                break;
            }
        }
    }

    void AnalysisRule(wNode* node, wNode* parent) {
        switch (node->data_.type) {
        case wTokenType::wID:
            target_token_.Insert(node->data_.value.ID, node); // TODO: add vector to save all node
            break;
        
        case wTokenType::wLetter: {
            char id_buf[33];
            sprintf_s(id_buf, "Sym%d\0", int(node->data_.value.Letter));
            lexer_token_.Insert(id_buf, node);
        } break;

        default: {
            for (size_t i = 0; i < node->kids_.Size(); ++i) {
                AnalysisRule(node->kids_[i], node);
            }
        } break;
        }

        // Check not branches [ ]?
        if ((int)node->data_.type >= (int)wTokenType::wBlockQ && (int)node->data_.type <= (int)wTokenType::wBlockQ01) {
            bool branches = false;
            for (size_t i = 0; i < node->kids_.Size(); ++i) {
                branches |= node->kids_[i]->data_.type == wTokenType::wVertLine;
            }

            if (!branches) {
                wLogger.PrintWarning(InputFileName, node->data_.line, node->data_.column, "find %s without branches; no branches, no fire", node->data_.GetTypeName());

                if (node->data_.type != wTokenType::wBlockQ) {
                    node->data_.type = wTokenType((int)node->data_.type - 4);
                } else {
                    node->data_.type = wTokenType::wBlock;
                }

                return;
            }
        }
    }

private:
    bool LL1_state = true;

    LL1Info* StartLL1Check(Set<MapString32>&general, wNode* node) {
        if (!LL1_state) (assert(0 && "Fail")); // TODO: to exeption

        //wLogger.PrintDebug("In check %s", node->data_.GetTypeName());
        //wLogger.PrintDebug("In check");

        switch (node->data_.type) {
        case wTokenType::wNone: {
            assert(0);
        } break;

        case wTokenType::wRule: {
            if (!general.Insert(node->kids_[0]->data_.value.ID)) {
                wLogger.PrintError("self challenge detected <%s>, the wolf bit his tail", node->data_.value.ID);
                LL1_state = false;
            }

            if (node->info_ != nullptr) {
                return node->info_;
            }

            node->info_ = new LL1Info;

            bool new_rule = true;
            bool maybe_empty = false;
            for (size_t i = 1; i < node->kids_.Size(); ++i) {
                if (node->kids_[i]->data_.type == wTokenType::wVertLine) {
                    node->kids_[i]->info_ = new LL1Info;
                    new_rule = true;
                    node->info_->maybe_empty |= maybe_empty;
                    maybe_empty = false;
                    continue;
                }

                if (new_rule) {
                    LL1Info* info = StartLL1Check(general, node->kids_[i]);
                    //if (!general.Combining(info->next)) {
                        //wLogger.PrintError("infinite recursion in <%s> with <%s>, the wolf bit his tail", node->data_.value.ID, node->kids_[i]->data_.value.ID);
                        //LL1_state = false;
                    ///}

                    if (!node->info_->next.Combining(info->next)) {
                        wLogger.PrintError("same start rules in <%s>, not LL1 language, wolf not wolf", node->data_.value.ID);
                        LL1_state = false;
                    }

                    if (info->maybe_empty) {
                        maybe_empty = true;
                    } else {
                        new_rule = false;
                        maybe_empty = false;
                    }
                } else {
                    Set<MapString32> s;
                    StartLL1Check(s, node->kids_[i]);
                }
            }

            node->info_->maybe_empty |= maybe_empty;
        } break;

        case wTokenType::wID: {
            node->info_ = new LL1Info;

            //wLogger.PrintDebug("ID: <%s>", node->data_.value.ID);
            auto res = rule_token_.Find(node->data_.value.ID);

            if (!res) {
                node->info_->next.Insert(node->data_.value.ID);
            } else {
                LL1Info* info = StartLL1Check(general, *res);
                if (!node->info_->next.Combining(info->next)) {
                    wLogger.PrintError("RECURSIV");
                    LL1_state = false;
                }
                
                if (info->maybe_empty) {
                    node->info_->maybe_empty = true;
                }
            }
        } break;

        case wTokenType::wLetter: {
            node->info_ = new LL1Info;
            char id_buf[33];
            sprintf_s(id_buf, "Sym%d\0", int(node->data_.value.Letter));
            node->info_->next.Insert(id_buf);
        } break;

        case wTokenType::wCode: {
            node->info_ = new LL1Info;
        } break;
        
        default:
            break;
        }

        if ((int)node->data_.type >= (int)wTokenType::wBlock && (int)node->data_.type <= (int)wTokenType::wBlockQ01) {\
            node->info_ = new LL1Info;
            
            bool new_rule = true;
            bool maybe_empty = false;
            for (size_t i = 0; i < node->kids_.Size(); ++i) {
                if (node->kids_[i]->data_.type == wTokenType::wVertLine) {
                    node->kids_[i]->info_ = new LL1Info;
                    new_rule = true;
                    node->info_->maybe_empty |= maybe_empty;
                    maybe_empty = false;
                    continue;
                }

                if (new_rule) {
                    LL1Info* info = StartLL1Check(general, node->kids_[i]);
                    //if (!general.Combining(info->next)) {
                    //    wLogger.PrintError("same start rules, not LL1 language, wolf not wolf");
                    //    LL1_state = false;
                    //}

                    if (!node->info_->next.Combining(info->next)) {
                        wLogger.PrintError("same start rules, not LL1 language, wolf not wolf");
                        LL1_state = false;
                    }

                    if (info->maybe_empty) {
                        maybe_empty = true;
                    } else {
                        new_rule = false;
                        maybe_empty = false;
                    }
                } else {
                    Set<MapString32> s;
                    StartLL1Check(s, node->kids_[i]);
                }
            }

            node->info_->maybe_empty |= maybe_empty;

            if (node->data_.type == wTokenType::wBlock01 
             || node->data_.type == wTokenType::wBlockQ01
             || node->data_.type == wTokenType::wBlock0N
             || node->data_.type == wTokenType::wBlockQ0N) {
                node->info_->maybe_empty = true;
            }
        }

        return node->info_;
    }
};

#endif // WOLF_PARSER_HPP