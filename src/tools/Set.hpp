#ifndef WOLF_SET_HPP
#define WOLF_SET_HPP

template <typename T>
class Set {
private:
    size_t size_ = 0;

    struct SetNode {
        T value;
        SetNode* left = nullptr;
        SetNode* right = nullptr;
        SetNode* parent = nullptr;

        SetNode(const T& val, SetNode* parent) : value(val), parent(parent) {}

        ~SetNode() {
            delete left;
            delete right;
        }
    };

    SetNode* head_ = nullptr;

    void CopyTree(SetNode* parent, SetNode*& dst_node, SetNode* src_node) {
        if (src_node == nullptr) return;

        dst_node = new SetNode(dst_node->value, parent);

        CopyTree(dst_node, dst_node->left, src_node->left);
        CopyTree(dst_node, dst_node->right, src_node->right);
    }
public:
    Set() {}

    Set(const Set& set) {
        CopyTree(nullptr, head_, set.head_);
    }

    Set(Set&& set) {
        std::swap(size_, set.size_);
        std::swap(head_, set.head_);
    }

    ~Set() {
        delete head_;
    }

    void Insert(const T& value) {
        SetNode* node = head_;
        SetNode* parent = head_;

        if (head_ == nullptr) {
            head_ = new SetNode(value, nullptr);
            return;
        }

        while (node != nullptr) {
            if (node->value == value)
                return;

            parent = node;

            if (value < node->value) 
                node = node->left;
            else
                node = node->right;
        }
        
        if (value < parent->value) {
            parent->left = new SetNode(value, parent);
            assert(parent->left);
        } else {
            parent->right = new SetNode(value, parent->parent);
            assert(parent->right);
        }

        ++size_;
    }

    bool Find(const T& value) const {
        SetNode* node = head_;

        while (node != nullptr){
            if (node->value == value) return true;

            if (value < node->value) 
                node = node->left;
            else 
                node = node->right;
        }
        
        return false;
    }

    inline size_t Size() const { return size_; }

    Set Intersection(const Set& other_set) {
        assert(0 && "TODO");
        /*const Set<T>* s1, * s2;
        if (Size() < other_set.Size()) {
            s1 = this;
            s2 = &other_set;
        } else {
            s1 = &other_set;
            s2 = this;
        }

        for (const auto& u : *s2) {
            if (s1->Find(u)) s.Insert(u);
        }

        return s;*/
    }

    void Combining(const Set& other_set) {
        if (other_set.Size() == 0) return;
        for (auto u : other_set) {
            //Insert(u);
            //printf("OK\n");
        }
    }

    /*#define DUMP_PATH "doc/dump/"
    #define DOT_PATH "\"C:/Program Files/Graphviz/bin/dot.exe\""
    void GraphicsDump(SetNode* node, FILE* file = nullptr) const {
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
        if (node == nullptr) return;
        fprintf(file, "Node%llu[shape=\"rectangle\", color=black, label=\"ID:%s\"];\n", node, wToken(0,0, node->value).GetTypeName());
        
        if (node->left) {
            fprintf(file, "Node%llu->Node%llu[dir=both];\n", node, node->left);
            GraphicsDump(node->left, file);
        }
        if (node->right) {
            fprintf(file, "Node%llu->Node%llu[dir=both];\n", node, node->right);
            GraphicsDump(node->right, file);
        }
        
        // -----------------------------------------------

        if (graph_file == nullptr) {
            fprintf(file, "}");

            fclose(file);

            char command_buffer[256] = {};
            sprintf(command_buffer, DOT_PATH " -O %s -T png", file_name);
            system(command_buffer);
        }
    }*/

protected:
    class SetIterator : public std::iterator<std::input_iterator_tag, T> {
    public:
        SetIterator(SetNode* data_ptr) : data_ptr_(data_ptr) {}
        SetIterator(const SetIterator& it) : data_ptr_(it.data_ptr_) {}

        bool operator!=(SetIterator const& other) const { return data_ptr_ != other.data_ptr_; }
        bool operator==(SetIterator const& other) const { return data_ptr_ == other.data_ptr_; }

        typename SetIterator::reference operator*() const { return data_ptr_->value; }

        SetIterator& operator++() {
            if (data_ptr_->right != nullptr) {
                data_ptr_ = data_ptr_->right;
                while (data_ptr_->left != nullptr) {
                    data_ptr_ = data_ptr_->left;
                }
                
            } else data_ptr_ = data_ptr_->parent;
            return *this;
        }

    private:
        SetNode* data_ptr_;
    };
public:
    using iterator = SetIterator;
    using const_iterator = SetIterator;

    iterator begin() {
        if (head_ == nullptr) return nullptr;

        SetNode* node = head_;
        while (node->left != nullptr) {
            node = node->left;
        }
        
        return node;
    }

    iterator end() {
        return nullptr;
    }

    const_iterator begin() const {
        if (head_ == nullptr) return nullptr;

        SetNode* node = head_;
        while (node->left != nullptr) {
            node = node->left;
        }
        
        return node;
    }

    const_iterator end() const {
        return nullptr;
    }
};

#endif // WOLF_SET_HPP