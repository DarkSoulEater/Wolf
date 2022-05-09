#ifndef WOLF_GENERATOR_HPP
#define WOLF_GENERATOR_HPP

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <cstddef>

#include "Tree.hpp"
#include "Token.hpp"
#include "Parser.hpp"

static const char* License = "/* This code is generated automatically!\n*\n*/\n\n";

static const char* CoreHeader = "#include \"core/WolfCore.hpp\"\n\n";

static const char* TypesComment = "// Wolf types\n";

static const char* UnionDecl = "union wTokenUnion {";

static const char* RulesComment = "// Wolf Rules\n";

static const char* StartParseComment = "// The main wolf parser function\n";

static const char* StartParse = "wToken StartParse() {";

#define INDENT(CNT)

class wGenerator {
private:
    wNode* root_;

    wParser<wToken>& parser_;
public:
    wGenerator(wNode* root, wParser<wToken>& parser) : root_(root), parser_(parser) {}
    ~wGenerator() {}

    void Generate(const char* out_file_name) {
        fopen_s(&file_rule, "C:/Users/eleno/C++/Wolf/wolf_parser/core/ParserRule.decl", "w");
        if (file_rule == nullptr) {
            wLogger.PrintError("filed to open rule file");
            perror("        ");
            return;
        }

        fopen_s(&file_token, "C:/Users/eleno/C++/Wolf/wolf_parser/core/ParserToken.decl", "w");
        if (file_token == nullptr) {
            wLogger.PrintError("filed to open token file");
            perror("        ");
            return;
        }

        fopen_s(&file_token_type, "C:/Users/eleno/C++/Wolf/wolf_parser/core/TokenType.hpp", "w");
        if (file_token_type == nullptr) {
            wLogger.PrintError("filed to open token type file");
            perror("        ");
            return;
        }

        fopen_s(&file, out_file_name, "w");
        if (file == nullptr) {
            wLogger.PrintError("%s failed to open file", out_file_name);
            perror("        ");
            return;
        }

        fprintf(file, License);
        fprintf(file, TypesComment);
        // Enum
        fprintf(file_token_type, "#ifndef WOLF_TOKEN_TYPE_HPP_\n");
        fprintf(file_token_type, "#define WOLF_TOKEN_TYPE_HPP_\n\n");
        fprintf(file_token_type, "#include \"Tree.hpp\"\n\n");
        fprintf(file_token_type, "enum wTokenType {\n");
        fprintf(file_token_type, "    wNone,\n");
        fprintf(file_token_type, "    wLexerError,\n");
        fprintf(file_token_type, "    wFail,\n");
        fprintf(file_token_type, "    wEOF,\n");
        for (const auto& u : parser_.lexer_token_) {
            fprintf(file_token_type, "    w%s,\n", u.first.str);
            fprintf(file_token, "DeclLexerTarget(%s)\n", u.first.str);
        }

        for (const auto& u : parser_.rule_token_) {
            fprintf(file_token_type, "    w%s,\n", u.first.str);
        }
        fprintf(file_token_type, "};\n\n");

        // Union
        fprintf(file_token_type, "union wTokenUnion {\n");
        for (size_t j = 0; j < root_->kids_.Size(); ++j) {
            if (root_->kids_[j]->data_.type == wTokenType::wDeclarations) {
                wNode* node = root_->kids_[j];
                for (size_t i = 0; i < node->kids_.Size(); i += 2) {
                    if (node->kids_[i + 1]->data_.type == wTokenType::wCode) {
                        wNode* code = node->kids_[i + 1];
                        for (size_t k = 0; k < code->kids_.Size(); ++k) {
                            fprintf(file_token_type, "    %s\n", code->kids_[k]->data_.value.str);
                        }
                        break;
                    }
                }

                break;
            }
        }
        fprintf(file_token_type, "};\n\n");

        fprintf(file_token_type, "static const char* TokenTypeGetName(wTokenType type) {\n");
        fprintf(file_token_type, "    switch (type) {\n");
        for (const auto& u: parser_.lexer_token_) {
            fprintf(file_token_type, "    case wTokenType::w%s:\n", u.first.str);
            if (u.first.str[0] == 'S' && u.first.str[1] == 'y' && u.first.str[2] == 'm') {
                int num = 0;
                sscanf(&u.first.str[3], "%d", &num);
                fprintf(file_token_type, "         return \"%c\";\n\n", char(num));
            } else {
                fprintf(file_token_type, "         return \"%s\";\n\n", u.first.str);
            }
        }
        for (const auto& u: parser_.rule_token_) {
            fprintf(file_token_type, "    case wTokenType::w%s:\n", u.first.str);
            fprintf(file_token_type, "         return \"%s\";\n\n", u.first.str);
        }
        fprintf(file_token_type, "    default:{}\n");
        fprintf(file_token_type, "    }\n");
        fprintf(file_token_type, "}\n");

        fprintf(file_token_type, "#endif // WOLF_TOKEN_TYPE_HPP_\n");

        fprintf(file, CoreHeader);
        fprintf(file, RulesComment);

        wNode* node = root_;
        for (size_t i = 0; i < node->kids_.Size(); ++i) {
            if (node->kids_[i]->data_.type == wTokenType::wRule) {
                PrintRule(node->kids_[i], 4);
            }
        }

        fprintf(file, StartParseComment);
        Print(0, "wToken wParser::StartParse() {");
        Print(4, "auto res_parse = Get%s();", parser_.start_rule_name);
        Print(4, "auto token = lexer_.ViewToken();\n");
        Print(4, "if (syntax_state_ == false || token.type != wTokenType::wEOF)");
        Print(8, "return wToken(wTokenType::wFail);\n");
        Print(4, "return res_parse;");
        Print(0, "}\n");

        Print(0, "// Lexical analysis");
        Print(0, "wToken wLexer::Lex() {");
        Print(4, "// Lexer code here");
        Print(0, "}");

        fclose(file);
    }

private:
    FILE* file;
    FILE* file_rule;
    FILE* file_token;
    FILE* file_token_type;

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

    void Print_(size_t indent, const char* format, ...) { // Not \n
        assert(format);

        for (size_t i = 0; i < indent; ++i) {
            fprintf(file, " ");
        }

        va_list message;
        va_start(message, format);
        vfprintf(file, format, message);
    }

    #define PrintView()                                 \
        size_t size = node->info_->next.Size();         \
        size_t i = 0;                                   \
        for (const auto& u : node->info_->next) {       \
            fprintf(file, "View(%s)", u.str);           \
            if (++i < size) fprintf(file, " || ");    \
        }

    #define IntrPrintView()                             \
        size = node->info_->next.Size();                \
        i = 0;                                          \
        for (const auto& u : node->info_->next) {       \
            fprintf(file, "View(%s)", u.str);           \
            if (++i < size) fprintf(file, " || ");    \
        }

    #define PrintKid()                                                          \
            indent += 4;                                                        \
            Print(indent, "switch (token.type) {");                      \
            bool new_rule = true;                                               \
            for (size_t i = 0; i < node->kids_.Size(); ++i) {                   \
                if (node->kids_[i]->data_.type == wTokenType::wVertLine) {      \
                    new_rule = true;                                            \
                    continue;                                                   \
                }                                                               \
                                                                                \
                if (new_rule) {                                                 \
                    assert(node->kids_[i]->info_);                              \
                    if (i != 0) {                                               \
                        Print(indent, "} break;\n");                            \
                        Print(0, "");                                           \
                    }                                                           \
                    for (size_t j = i; j < node->kids_.Size(); ++j) {           \
                        for (const auto& u : node->kids_[j]->info_->next) {     \
                            Print(indent, "case wTokenType::w%s:", u.str);       \
                        }                                                       \
                        if (!node->kids_[j]->info_->maybe_empty) break;         \
                    }                                                           \
                    Print(indent, "{");                                         \
                    new_rule = false;                                           \
                }                                                               \
                                                                                \
                PrintRule(node->kids_[i], indent + 4);                          \
            }                                                                   \
            Print(indent, "} break;\n");                                        \
            Print(indent, "default:{error(\"\");}");                                          \
            Print(indent, "}\n");                                               \
            indent -= 4;

    void PrintRule(wNode* node, size_t indent) {
        switch (node->data_.type) {
        case wTokenType::wRule: {
            fprintf(file_rule, "wToken Get%s();\n", node->kids_[0]->data_.value.ID);
            Print(0, "wToken wParser::Get%s() {", node->kids_[0]->data_.value.ID);
            Print(4, "InitRule(%s);\n", node->kids_[0]->data_.value.ID);

            Print(indent, "switch (token.type) {");
            bool new_rule = true;
            for (size_t i = 1; i < node->kids_.Size(); ++i) {
                if (node->kids_[i]->data_.type == wTokenType::wVertLine) {
                    new_rule = true;
                    continue;
                }

                if (new_rule) {
                    assert(node->kids_[i]->info_);
                    if (i != 1) {
                        Print(indent, "} break;\n");
                        Print(0, "");
                    }
                    for (size_t j = i; j < node->kids_.Size(); ++j) {
                        for (const auto& u : node->kids_[j]->info_->next) {
                            Print(indent, "case wTokenType::w%s:", u.str);
                        }
                        if (!node->kids_[j]->info_->maybe_empty) break;
                    }
                    Print(indent, "{");
                    new_rule = false;
                }

                PrintRule(node->kids_[i], indent + 4);
            }
            Print(indent, "} break;\n");
            Print(indent, "default:{error(\"\");}");
            Print(indent, "}\n");

            Print(4, "Success();");
            Print(0, "}\n");
        } break;

        case wTokenType::wBlock: {
            for (size_t i = 0; i < node->kids_.Size(); ++i) {
                PrintRule(node->kids_[i], indent);
            }
        } break;

        case wTokenType::wBlock01: { // [ ]~
            Print_(indent, "if (");
            PrintView();
            Print(0, ") { // [ ]~");
            PrintKid();
            Print(indent, "}"); 
        } break;

        case wTokenType::wBlock0N: { // [ ]*
            Print_(indent, "while (");
            PrintView();
            Print(0, ") { // [ ]*");
            PrintKid();
            Print(indent, "}");
        } break;

        case wTokenType::wBlock1N: { // [ ]+
            Print_(indent, "if (!(");
            PrintView();
            Print(0, ")) // [ ]+");
            Print(indent + 4, "error(\"\")");

            Print_(indent, "while (");
            IntrPrintView();
            Print(0, ") { // [ ]*");
            PrintKid();
            Print(indent, "}");
        } break;


    #define PrintQ()                                                    \
        Print(indent, "switch (token.type) { // [ ]?");         \
                                                                        \
        bool new_rule = true;                                           \
        for (size_t i = 0; i < node->kids_.Size(); ++i) {               \
            if (node->kids_[i]->data_.type == wTokenType::wVertLine) {  \
                new_rule = true;                                        \
                continue;                                               \
            }                                                           \
                                                                        \
            if (new_rule) {                                             \
                assert(node->kids_[i]->info_);                          \
                if (i != 0) {                                           \ 
                    Print(indent, "} break;\n");                        \
                }                                                       \
                for (size_t j = i; j < node->kids_.Size(); ++j) {       \
                    for (const auto& u : node->kids_[j]->info_->next) { \
                        Print(indent, "case wTokenType::w%s:", u.str);   \
                    }                                                   \
                    if (!node->kids_[j]->info_->maybe_empty) break;     \
                }                                                       \
                Print(indent, "{");                                     \
                new_rule = false;                                       \
            }                                                           \
                                                                        \
            PrintRule(node->kids_[i], indent + 4);                      \
        }                                                               \
        Print(indent, "} break;\n");                                    \
        Print(indent, "default:{error(\"\");}");                                    \
        Print(indent, "}\n");                                           

        case wTokenType::wBlockQ01: { // [ ]?~
            Print_(indent, "if (");
            PrintView();
            Print(0, ") { // [ ]~");
            indent += 4;
            PrintQ();
            indent -= 4;
            Print(indent, "}"); 
        } break;

        case wTokenType::wBlockQ0N: { // [ ]?*
            Print_(indent, "while (");
            PrintView();
            Print(0, ") { // [ ]*");
            indent += 4;
            PrintQ();
            indent -= 4;
            Print(indent, "}");
        } break;

        case wTokenType::wBlockQ1N: { // [ ]?+
            Print_(indent, "if (!(");
            PrintView();
            Print(0, ")) // [ ]+");
            Print(indent + 4, "error(\"\")");

            Print_(indent, "while (");
            IntrPrintView();
            Print(0, ") { // [ ]*");
            indent += 4;
            PrintQ();
            indent -= 4;
            Print(indent, "}");
        } break;

        case wTokenType::wBlockQ: { // [ ]?
            PrintQ();
        } break;

        case wTokenType::wID: {
            Print(indent, "Get(%s);", node->data_.value.ID);
        } break;

        case wTokenType::wLetter: {
            char buf[32] = {};
            sprintf_s(buf, "Get(Sym%d);\0", (int)node->data_.value.Letter);
            Print(indent, buf, node->data_.value.Letter);
        } break;

        case wTokenType::wCode: {
            Print(indent, "{");
            for (size_t i = 0; i < node->kids_.Size(); ++i) {
                Print(indent + node->kids_[i]->kids_[0]->data_.value.Indent * 4, 
                      "%s", node->kids_[i]->data_.value.str);
            }
            Print(indent, "}");
        } break;
        
        default:
            wLogger.PrintError("strange token %s", node->data_.GetTypeName());
            break;
        }
    }

private: 
    void GenerateHpp(const char* file_name) {
        std::ifstream src("C:/Users/eleno/C++/Wolf/core_data/Lexer.txt", std::ios::binary);
        if (!src.is_open()) {
            std::cerr << "Open Lexer fail\n";
        }

        std::ofstream dest("C:/Users/eleno/C++/Wolf/doc/res/core.txt", std::ios::binary);
        if (!dest.is_open()) {
            std::cerr << "Open dest fail\n";
        }

        dest << src.rdbuf();

        //src.open("C:/Users/eleno/C++/Wolf/core_data/Tree.txt", std::ios::binary);
        //if (!src.is_open()) {
        //    std::cerr << "Open Tree fail\n";
        //}
        //dest << src.rdbuf();
        return;


        fopen_s(&file, "C:/Users/eleno/C++/Wolf/doc/res/core.txt", "w");

        if (file == nullptr) {
            wLogger.PrintError("%s failed to open file", "core");
            perror("        ");
            return;
        }

        Print(0, "Core Ok");

        
        

        fclose(file);
    }
};

#endif // WOLF_GENERATOR_HPP