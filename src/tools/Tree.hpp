#ifndef WOLF_TREE_HPP_
#define WOLF_TREE_HPP_

#include <assert.h>
#include <Vector.hpp>

template<typename T>
struct Node {
    T data_;
    Vector<Node*> childs_;
};

template<typename T>
class Tree {
private:
    using TNode = Node<T>;

    TNode* head_;
    size_t size_;
    
public:
    Tree() : head_(nullptr), size_(0) {

    }

    ~Tree() {
        // TODO: !!!
    }

    void Insert(TNode* node, const T& value) {
        TNode* new_node = new TNode;
        new_node->data_ = value;

        if (node == nullptr) {
            if (head_ != nullptr) {
                new_node->childs_.PushBack(head_);
            }
            head_ = new_node;
        } else {
            node->childs_.PushBack(new_node);
        }

        ++size_;
    }

    void Insert(TNode* node, TNode* new_node, bool is_new_node = true) {
        if (new_node == nullptr) return;
        assert(new_node);

        if (node == nullptr) {
            if (head_ != nullptr) {
                new_node->childs_.PushBack(head_);
            }
            head_ = new_node;
        } else {
            node->childs_.PushBack(new_node);
        }

        if (is_new_node) ++size_;
    }

    void ClearSubTree(TNode* node) {
        assert(node != nullptr);
        
        for (size_t i = 0; i < node->childs_.Size(); ++i) {
            ClearSubTree_(node->childs_[i]);
        }
        node->childs_.Clear();
    }

    #define DUMP_PATH "doc/dump/"
    #define DOT_PATH "\"C:/Program Files/Graphviz/bin/dot.exe\""

    void GraphicsDump() const {
        char file_name[128] = {};

        static size_t dump_id = 0;
        sprintf(file_name, DUMP_PATH  "log_list-dump_%llu", dump_id++);

        FILE *graph_file = nullptr;
        if (fopen_s(&graph_file, file_name, "w")) {
            perror("Could not open file\n");
            return;
        }

        fprintf(graph_file, "digraph G{\nsearchsize=100\n");

        GraphicsDump(graph_file, head_);

        fprintf(graph_file, "}");

        fclose(graph_file);

        char command_buffer[256] = {};
        sprintf(command_buffer, DOT_PATH " -O %s -T png", file_name);
        system(command_buffer);
    }

    inline size_t Size() const { return size_; }

    inline TNode* GetRoot() { return head_; }

private:
    void GraphicsDump(FILE *file, TNode* node) const {
        if (node == nullptr) assert(0);

        if (node->data_.type == TokenType::ID) {
            fprintf(file, "Node%llu[shape=\"rectangle\", color=black, label=\"%s:%s\"];\n", node, node->data_.GetTypeName(), node->data_.ID);
        } else if (node->data_.type == TokenType::Letter) {
            fprintf(file, "Node%llu[shape=\"rectangle\", color=black, label=\"%c\"];\n", node, node->data_.Letter);
        } else {
            fprintf(file, "Node%llu[shape=\"rectangle\", color=black, label=%s];\n", node, node->data_.GetTypeName());
        }

        for (size_t i = 0; i < node->childs_.Size(); ++i) {
            fprintf(file, "Node%llu->Node%llu[dir=both];\n", node, node->childs_[i]);
            GraphicsDump(file, node->childs_[i]);
        }
    }

    void ClearSubTree_(TNode* node) {
        assert(node != nullptr);

        for (size_t i = 0; i < node->childs_.Size(); ++i) {
            ClearSubTree_(node->childs_[i]);
        }
        delete node;
        --size_;
    }
};


#endif // WOLF_TREE_HPP_