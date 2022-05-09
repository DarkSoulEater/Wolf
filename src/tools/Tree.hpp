#ifndef WOLF_TREE_HPP_
#define WOLF_TREE_HPP_

#include <assert.h>
#include <iostream>

#include "Token.hpp"
#include "Vector.hpp"
#include "Type.hpp"
#include "Set.hpp"

struct LL1Info {
    Set<MapString32> next;
    bool maybe_empty = false;
};


template<typename T>
class Node {
public:
    T data_;
    Vector<Node*> kids_;
    LL1Info* info_ = nullptr;

    Node() {}
    Node(const T& data) : data_(data) {}

    ~Node() {
        for (size_t i = 0; i < kids_.Size(); ++i) {
            delete kids_[i];
        }
    }

    void Insert(Node* node) {
        kids_.PushBack(node);
    }

    void Insert(const T& data) {
        Node* node = new Node(data);
        kids_.PushBack(node);
    }

    void InsertKid(Node* node) {
        size_t size = node->kids_.Size();
        for (size_t i = 0; i < size; ++i) {
            kids_.PushBack(node->kids_[i]);
        }
    }

    void Print() const {
        std::cout << "Data:(" << this << ") ";
        data_.Print();
        std::cout << "Kids: ";
        for (size_t i = 0; i < kids_.Size(); ++i) {
            std::cout << kids_[i] << " ";
        }
        std::cout << "\n\n";
    }

    const char* GetTypeName() const {
        return data_.GetTypeName();
    }

    #define DUMP_PATH "doc/dump/"
    #define DOT_PATH "\"C:/Program Files/Graphviz/bin/dot.exe\""
    void GraphicsDump(FILE* file = nullptr) const {
        FILE *graph_file = file;
        char file_name[128] = {};

        if (file == nullptr) {
            static size_t dump_id = 0;
            sprintf(file_name, DUMP_PATH  "log_list-dump_%llu", dump_id++);

            if (fopen_s(&file, file_name, "w")) {
                perror("Could not open file\n");
                return;
            }
        

            fprintf(file, "digraph G{\nsearchsize=100\n");
        }

        // -----------------------------------------------
        const char* kColorBlack = "black";
        const char* kColorGrey = "grey";
        #define COLOR  ((info_ && info_->maybe_empty) ? kColorGrey : kColorBlack)

        if (data_.type == wTokenType::wID) {
            fprintf(file, "Node%llu[shape=\"rectangle\", color=%s, label=\"ID:%s\"];\n", this, COLOR, data_.value.ID);
        } else if (data_.type == wTokenType::wLetter) {
            if (data_.value.Letter == '\'' || data_.value.Letter == '\"' || data_.value.Letter == '\\') {
                fprintf(file, "Node%llu[shape=\"rectangle\", color=%s, label=\"\\%c\"];\n", this, COLOR, data_.value.Letter);
            } else {
                fprintf(file, "Node%llu[shape=\"rectangle\", color=%s, label=\"%c\"];\n", this, COLOR, data_.value.Letter);
            }
        } else {
            fprintf(file, "Node%llu[shape=\"rectangle\", color=%s, label=\"%s\"];\n", this, COLOR, GetTypeName());
        }

        if (data_.type == wTokenType::wNone) return;
        
        for (size_t i = 0; i < kids_.Size(); ++i) {
            fprintf(file, "Node%llu->Node%llu[dir=both];\n", this, kids_[i]);

            kids_[i]->GraphicsDump(file);
        }
        
        // -----------------------------------------------

        if (graph_file == nullptr) {
            fprintf(file, "}");

            fclose(file);

            char command_buffer[256] = {};
            sprintf(command_buffer, DOT_PATH " -O %s -T png", file_name);
            system(command_buffer);
        }
    }
};

#endif // WOLF_TREE_HPP_