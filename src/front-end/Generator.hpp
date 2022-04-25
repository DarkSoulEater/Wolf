#ifndef WOLF_GENERATOR_HPP
#define WOLF_GENERATOR_HPP

#include <iostream>
#include "Tree.hpp"
#include <Token.hpp>

class Generator {
private:
    Tree<Token>& tree_;
    using gNode = Node<Token>;
public:
    Generator(Tree<Token>& tree) : tree_(tree) {}
    ~Generator() {}

    void Generate() {
        const char* file_name = "doc/res/out.txt";
        FILE* file = {};
        fopen_s(&file, file_name, "w");

        gNode* node = tree_.GetRoot();
        for (size_t i = 0; i < node->childs_.Size(); ++i) {
            if (node->childs_[i]->data_.type == TokenType::Rules_) {
                gNode* rools = node->childs_[i];
                for (size_t j = 0; j < rools->childs_.Size(); ++j) {
                    PrintRule(file, rools->childs_[j]);
                }
            }
        }

        fclose(file);
    }

private:
    void PrintRule(FILE* file, gNode* node) {
        fprintf(file, "void Get_%s() {\n}\n\n", node->childs_[0]->data_.ID);
    }
};

#endif // WOLF_GENERATOR_HPP