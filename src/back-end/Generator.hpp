#ifndef WOLF_GENERATOR_HPP
#define WOLF_GENERATOR_HPP

#include <iostream>
#include "Tree.hpp"
#include <Token.hpp>

static const char* License = "/* This code is generated automatically\n*\n*/\n\n";

static const char* CoreHeader = "#include \"WolfCore.hpp\"\n\n";

static const char* TypesComment = "// Wolf types\n";

static const char* EnumDecl = "enum wTokenType {";

static const char* UnionDecl = "union wTokenUnion {";

static const char* RulesComment = "// Wolf Rules\n";

static const char* StartParseComment = "// The main wolf parser function\n";

static const char* StartParse = "wToken StartParse() {";

/*
class Generator {
private:
    wNode* root_;
public:
    Generator(wNode* root) : root_(root) {}
    ~Generator() {}

    void Generate() {
        const char* file_name = "doc/res/out.txt";
        FILE* file = {};
        fopen_s(&file, file_name, "w");

        fprintf(file, License);
        fprintf(file, CoreHeader);
        fprintf(file, TypesComment);
        fprintf(file, "%s\n\n}\n\n", EnumDecl);
        fprintf(file, "%s\n\n}\n\n", UnionDecl);
        fprintf(file, RulesComment);

        wNode* node = root_;
        for (size_t i = 0; i < node->kids_.Size(); ++i) {
            if (node->kids_[i]->data_.type == wTokenType::wRules) {
                wNode* rools = node->kids_[i];
                for (size_t j = 0; j < rools->kids_.Size(); ++j) {
                    PrintRule(file, rools->kids_[j]);
                }
            }
        }

        fprintf(file, StartParseComment);
        fprintf(file, "%s\n\n}\n", StartParse);

        fclose(file);
    }

private:
    void PrintRule(FILE* file, wNode* node) {
        fprintf(file, "wToken Get%s() {\n", node->kids_[0]->data_.value.ID);

        int max_cnt_depend = 0;
        for (size_t i = 1; i < node->kids_.Size(); ++i) {
            int kid_depend = node->kids_[i]->kids_.Size();
            if (node->kids_[i]->kids_[kid_depend - 1]->data_.type == wTokenType::wCode) kid_depend -= 1;
            max_cnt_depend = std::max(max_cnt_depend, kid_depend);
        }
        GenerateInitial(file, max_cnt_depend + 1);

        size_t size = node->kids_.Size();
        for (size_t i = 1; i < size; ++i) {
            fprintf(file, "    // Branch %llu\n", i);
            GenerateDepend(file, node->kids_[i], node->kids_[0]->data_.value.ID);
            fprintf(file, "    wlexer.StateRollback(state);\n\n");
        }

        GenerateFinal(file);

        fprintf(file, "}\n\n");
    }

    void GenerateInitial(FILE* file, int max_cnt_depend) {
        static const char* Init = "    // Initial\n"                    \
                                  "    wState state = wlexer.State()\n" \
                                  "    wToken tk[%d] = {};\n\n"         ;

        fprintf(file, Init, max_cnt_depend);
    }

    void GenerateFinal(FILE* file) {
        static const char* Fail = "    // Fail final\n"                     \
                                  "    tk[0].type = wTokenType::wFail\n"    \
                                  "    return tk[0];\n"                     ;

        fprintf(file, Fail);
    }

    void GenerateDepend(FILE* file, wNode* node, const char* rule_name) {
        fprintf(file, "    if (");

        size_t size = node->kids_.Size();
        size_t i = 0;
        for (; i < size; ++i) {
            if (node->kids_[i]->data_.type == wTokenType::wCode) break;

            if (i != 0) fprintf(file, "\n     && ");

            if (node->kids_[i]->data_.type == wTokenType::wID) {
                fprintf(file, "(tk[%llu] = Get%s()).type == wTokenType::%s", i + 1, node->kids_[i]->data_.value.ID, node->kids_[i]->data_.value.ID);
            } else {
                fprintf(file, "(tk[%llu] = GetSym[%c]()).type == wTokenType::Sym[%c]", i + 1, node->kids_[i]->data_.value.Letter, node->kids_[i]->data_.value.Letter);
            }
        }
        fprintf(file, ") {\n");
        fprintf(file, "        tk[0].type = wTokenType::%s;\n", rule_name);

        if (i < size && node->kids_[i]->data_.type == wTokenType::wCode) {
            fprintf(file, "\n        { // ---------- User Code ----------\n\n");
            //fprintf(file, "            // User Code\n");
            //fprintf(file, "%s\n", node->kids_[i]->data_.value.Node);
            wNode* code = node->kids_[i];
            for (size_t j = 0; j < code->kids_.Size(); ++j) {
                for (size_t k = 0; k < code->kids_[j]->kids_[0]->data_.value.Indent; ++k)
                    fprintf(file, "    ");
                fprintf(file, "    %s\n", code->kids_[j]->data_.value.str);
            }
            fprintf(file, "\n        } // ---------- End  Code ----------\n\n");
        } else 
            fprintf(file, "\n");

        fprintf(file, "        return tk[0];\n");

        fprintf(file, "    }\n");
        ///fprintf(file, "    if () {\n\n    }\n\n");
    }
};*/

#endif // WOLF_GENERATOR_HPP