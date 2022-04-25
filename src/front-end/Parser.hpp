#ifndef WOLF_PARSER_HPP
#define WOLF_PARSER_HPP

#include <iostream>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Tree.hpp"
#include "Generator.hpp"

template<typename RetT, typename Token>
class Parser {
private:
    using pNode = Node<Token>;
    Tree<Token> tree_;

    Lexer<Token> lexer_;
    const size_t& lexer_state_;

    bool state_;
public:
    Parser() : lexer_state_(lexer_.GetState()), state_(true) {}

    ~Parser() {}

    #define A 10
    #if A < 12
    #endif
    #define TOKEN(TOKEN_NAME, LEVEL)                        \
        Token Get_##TOKEN_NAME() {                          \
            Token token = lexer_.GetToken();                \
            state_ = (token.type == TokenType::TOKEN_NAME); \
            return token;                                   \
        }                                                   \
//#endif
    #include "LexerToken.def" 

    pNode* Get_Code() {
        size_t save_state = lexer_state_;
        Token token = lexer_.GetToken();
        if (token.type == TokenType::Code_) {
            pNode* node = new pNode;
            node->data_ = token;
            return node;
        } else {
            lexer_.Rollback(save_state);
            return nullptr;
        }
    }      

    pNode* Get_Depend() {
        // Initial
        size_t save_state = lexer_state_;
        state_ = true;
        pNode* node = new pNode;
        node->data_.type = TokenType::Depend_;

        // GetID
        if (!state_) goto BRANCH_1;
        tree_.Insert(node, Get_ID());

        // GetDepend
        if (!state_) goto BRANCH_1;
        
        {
            pNode* depend_node = Get_Depend();
            if (state_) {
                for (size_t i = 0; i < depend_node->childs_.Size(); ++i) {
                    tree_.Insert(node, depend_node->childs_[i], false);
                }
            }
            delete depend_node;
        }

        // GetCode
        if (!state_) goto BRANCH_1;
        tree_.Insert(node, Get_Code());

        // Branch
        if (state_) return node;
        BRANCH_1:
        tree_.ClearSubTree(node);
        lexer_.Rollback(save_state);
        state_ = true;

        // GetLetter
        if (!state_) goto BRANCH_2;
        tree_.Insert(node, Get_Letter());

        // GetDepend
        if (!state_) goto BRANCH_2;
        
        {
            pNode* depend_node = Get_Depend();
            if (state_) {
                for (size_t i = 0; i < depend_node->childs_.Size(); ++i) {
                    tree_.Insert(node, depend_node->childs_[i], false);
                }
            }
            delete depend_node;
        }

        // GetCode
        if (!state_) goto BRANCH_2;
        tree_.Insert(node, Get_Code());

        // Branch
        if (state_) return node;
        BRANCH_2:
        tree_.ClearSubTree(node);
        lexer_.Rollback(save_state);
        state_ = true;

        // GetID
        if (!state_) goto BRANCH_3;
        tree_.Insert(node, Get_ID());

        // GetCode
        if (!state_) goto BRANCH_3;
        tree_.Insert(node, Get_Code());

        // Branch
        if (state_) return node;
        BRANCH_3:
        tree_.ClearSubTree(node);
        lexer_.Rollback(save_state);
        state_ = true;

        // GetLetter
        if (!state_) return node;
        tree_.Insert(node, Get_Letter());

        // GetCode
        if (!state_) goto BRANCH_4;
        tree_.Insert(node, Get_Code());

        // Terminate        
        if (state_) return node;
        BRANCH_4:
        tree_.ClearSubTree(node);
        delete node;
        return nullptr;
    }

    pNode* Get_Depends() {
        // Initial
        size_t save_state = lexer_state_;
        state_ = true;
        pNode* node = new pNode;
        node->data_.type = TokenType::Depends_;

        // GetDepend
        if (!state_) goto BRANCH_1;
        tree_.Insert(node, Get_Depend());

        // Require('|')
        if (!state_) goto BRANCH_1;
        Get_VertLine();

        // GetDepends
        if (!state_) goto BRANCH_1;

        {
            pNode* depends_node = Get_Depends();
            if (state_) {
                for (size_t i = 0; i < depends_node->childs_.Size(); ++i) {
                    tree_.Insert(node, depends_node->childs_[i], false);
                }
            }
            delete depends_node;
        }

        // Branch
        if (state_) return node;
        BRANCH_1:
        tree_.ClearSubTree(node);
        lexer_.Rollback(save_state);
        state_ = true;

        // GetDepend
        if (!state_) goto BRANCH_2;
        tree_.Insert(node, Get_Depend());
        
        // Terminate
        if (state_) return node;
        BRANCH_2:
        tree_.ClearSubTree(node);
        delete node;
        return nullptr;
    }

    pNode* Get_Rule() {
        // Initial
        size_t save_state = lexer_state_;
        state_ = true;
        pNode* node = new pNode;
        node->data_.type = TokenType::Rule_;

        // GetID
        if (!state_) goto BRANCH_1;
        tree_.Insert(node, Get_ID());

        // Require(':')
        if (!state_) goto BRANCH_1;
        Get_Colon();

        // GetDepends
        if (!state_) goto BRANCH_1;
        tree_.Insert(node, Get_Depends());

        // GetSemicolon
        if (!state_) goto BRANCH_1;
        Get_Semicolon();

        // Terminate
        if (state_) return node;
        BRANCH_1:
        tree_.ClearSubTree(node);
        delete node;
        return nullptr;
    }

    pNode* Get_Rules() {
        // Initial
        size_t save_state = lexer_state_;
        state_ = true;
        pNode* node = new pNode;
        node->data_.type = TokenType::Rules_;

        // GetRule
        if (!state_) goto BRANCH_1;
        tree_.Insert(node, Get_Rule());

        // GetRules
        if (!state_) goto BRANCH_1;

        {
            pNode* rules_node = Get_Rules();
            if (state_) {
                for (size_t i = 0; i < rules_node->childs_.Size(); ++i) {
                    tree_.Insert(node, rules_node->childs_[i], false);
                }
            }
            delete rules_node;
        }

        // Branch
        if (state_) return node;
        BRANCH_1:
        tree_.ClearSubTree(node);
        lexer_.Rollback(save_state);
        state_ = true;

        // GetRule
        if (!state_) goto BRANCH_2;
        tree_.Insert(node, Get_Rule());

        // Terminate
        if (state_) return node;
        BRANCH_2:
        tree_.ClearSubTree(node);
        delete node;
        return nullptr;
    }

    RetT Get_G() {
        // Initial
        size_t save_state = lexer_state_;
        state_ = true;
        pNode* node = new pNode;
        node->data_.type = TokenType::Program;

        if (!state_) goto BRANCH_1;
        tree_.Insert(node, Get_Rules());

        if (state_) { 
            tree_.Insert(nullptr, node);
            std::cout << tree_.Size() << "\n";
            tree_.GraphicsDump();

            Generator gen(tree_);
            gen.Generate();
        }
        BRANCH_1:
        tree_.ClearSubTree(node);
        delete node;
        //Get_EndOfFile();
        return state_;
    }

    RetT ParseFile(const char* file_name) {
        if (!lexer_.OpenFile(file_name)) {
            abort(); // TODO:
        }
        return Get_G();
    }
};

#endif // WOLF_PARSER_HPP