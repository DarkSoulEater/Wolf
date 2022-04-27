#include <iostream>
#include "Token.hpp"
#include "Parser.hpp"
#include <string.h>

void ParseArgs(const int argc, const char* argv[]);

#include <Vector.hpp>
#include "HashTable.hpp"
#include "HashTableKeyword.hpp"

struct Key {
    char key_[32];

    bool operator==(const Key& key) {
        return strcmp(key.key_, key_) == 0;
    }
};


int main0();

int main(const int argc, const char* argv[]) {
    /*main0();
    return 0;
    HashTableKeyword ht_key;
    ht_key.Insert(wKeyword("awdawd"));

    auto res = ht_key.Find(wKeyword("awdawd"));
    if (res) {
        std::cout << res->GetKey().val_ << "\n";
    } else {
        std::cout << "None\n";
    }
    
    /*return 0;
    HashTable<int, HashTableCell<int, Hash<int>>, Hash<int>, HashTableGrower<>> h;
    //h.Insert(10);

    Vector<int> v;

    for (size_t i = 0; i < 10240; ++i) {
        int val = rand();
        v.PushBack(val);
        h.Insert(val);
    }

    int cnt = 0;
    for (size_t i = 0; i < v.Size(); ++i) {
        std::cout << v[i] << " = ";
        auto res = h.Find(v[i]);
        if (res) {
            ++cnt;
            std::cout << res->GetKey() << "\n";
        } else {
            std::cout << "None\n";
        }
    }
    std::cout << "cnt = " <<  cnt << "\n";
    std::cout << "Size = " << v.Size();

    std::cout << "\n\n";

    for (size_t i = 0; i < 0; ++i) {
        int val = rand();
        std::cout << val << " = ";
        auto res = h.Find(val);
        if (res) {
            std::cout << res->GetKey() << "\n";
        } else {
            std::cout << "None\n";
        }
    }
    return 0;

    auto res = h.Find(11);
    if (res) {
        std::cout << res->GetKey() << "\n";
    } else {
        std::cout << "None\n";
    }*/

    ParseArgs(argc, argv);
    
    wParser<wToken> gen("test/simple_grammar.txt");
    gen.StartParce();
    //int val = gen.ParseFile("test/simple_grammar.txt");
    //std::cout << val;

    return 0;
}

void ParseArgs(const int argc, const char* argv[]) {
    for (int i = 0; i < argc; ++i) {
        printf("%s ", argv[i]);
    }
    printf("\n\n");
}