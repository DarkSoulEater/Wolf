#ifndef WOLF_GENERATOR_HPP
#define WOLF_GENERATOR_HPP

#include <iostream>
#include <Windows.h>
#include <cstddef>

#include "Tree.hpp"
#include "Token.hpp"

static const char* License = "/* This code is generated automatically\n*\n*/\n\n";

static const char* CoreHeader = "#include \"WolfCore.hpp\"\n\n";

static const char* TypesComment = "// Wolf types\n";

static const char* EnumDecl = "enum wTokenType {";

static const char* UnionDecl = "union wTokenUnion {";

static const char* RulesComment = "// Wolf Rules\n";

static const char* StartParseComment = "// The main wolf parser function\n";

static const char* StartParse = "wToken StartParse() {";

#define INDENT(CNT)

class wGenerator {
private:
    wNode* root_;
public:
    wGenerator(wNode* root) : root_(root) {}
    ~wGenerator() {}

    void Generate(const char* out_file_name) {
        fopen_s(&file, out_file_name, "w");

        if (file == nullptr) {
            wLogger.PrintError("%s failed to open file", out_file_name);
            perror("        ");
            return;
        }

        fprintf(file, License);
        fprintf(file, CoreHeader);
        fprintf(file, TypesComment);
        fprintf(file, "%s\n\n}\n\n", EnumDecl);
        fprintf(file, "%s\n\n}\n\n", UnionDecl);
        fprintf(file, RulesComment);

        wNode* node = root_;
        for (size_t i = 0; i < node->kids_.Size(); ++i) {
            if (node->kids_[i]->data_.type == wTokenType::wRule) {
                PrintRule(node->kids_[i], 0);
            }
        }
        /*for (size_t i = 0; i < node->kids_.Size(); ++i) {
            if (node->kids_[i]->data_.type == wTokenType::wRule) {
                wNode* rools = node->kids_[i];
                for (size_t j = 0; j < rools->kids_.Size(); ++j) {
                    PrintRule(file, rools->kids_[j]);
                }
            }
        }*/

        fprintf(file, StartParseComment);
        fprintf(file, "%s\n\n}\n", StartParse);

        fclose(file);
    }

private:
    FILE* file;

    void Print(size_t indent, const char* format, ...) {
        assert(format);

        for (size_t i = 0; i < indent; ++i) {
            fprintf(file, " ");
        }

        va_list message;
        va_start(message, format);
        vfprintf(file, format, message);
        fprintf(file, "\n");
    }

    void PrintRule(wNode* node, size_t indent) {
        switch (node->data_.type) {
        case wTokenType::wRule: {
            Print(0, "wToken wParser::Get%s() {", node->kids_[0]->data_.value.ID);
            Print(4, "Init(%s);\n", node->kids_[0]->data_.value.ID);

            for (size_t i = 1; i < node->kids_.Size(); ++i) {
                PrintRule(node->kids_[i], 4);
            }

            Print(0, "");
            Print(4, "Success();");
            Print(0, "}\n");
        } break;

        case wTokenType::wBlock: {
            for (size_t i = 0; i < node->kids_.Size(); ++i) {
                PrintRule(node->kids_[i], indent);
            }
        } break;

        case wTokenType::wBlock01: { // [ ]~

        } break;

        case wTokenType::wBlock0N: { // [ ]*

        } break;

        case wTokenType::wBlock1N: { // [ ]+

        } break;

        case wTokenType::wBlockQ01: { // [ ]?~

        }

        case wTokenType::wBlockQ0N: { // [ ]?*

        }

        case wTokenType::wBlockQ1N: { // [ ]?+

        } break;

        case wTokenType::wBlockQ: { // [ ]?

        } break;

        case wTokenType::wID: {
            Print(indent, "Get(%s);", node->data_.value.ID);
        } break;

        case wTokenType::wLetter: {
            Print(indent, "GetLetter(%c);", node->data_.value.Letter);
        } break;

        case wTokenType::wCode: {
            Print(indent, "{");
            Print(indent + 4, "// Code");
            Print(indent, "}");
        } break;
        
        default:
            wLogger.PrintError("strange token %s", node->data_.value.ID);
            break;
        }
    }
};

#endif // WOLF_GENERATOR_HPP