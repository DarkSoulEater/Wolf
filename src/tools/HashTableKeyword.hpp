#ifndef WOLF_HASH_TABLE_HPP
#define WOLF_HASH_TABLE_HPP

#include <string.h>
#include <immintrin.h>
#include "HashTable.hpp"

/*
struct wKeyword {
    char val_[32] = {};

    wKeyword() = default;
    wKeyword(const char* val) {
        strcpy_s(val_, val);
    }

    bool operator==(const wKeyword& rhs) const {
        return strcmp(val_, rhs.val_) == 0;
    }
};

struct wKeywrodHash {
    size_t operator() (const wKeyword& key) const {
        const char* val = key.val_;
        unsigned __int64 hash = 0;
        for (size_t i = 0; i < 4; ++i) {
            if (*((const unsigned __int64*)val + i) == 0ull) break;
            hash = _mm_crc32_u64(hash, *((const unsigned __int64*)val + i));
        }
        return (size_t)hash;
    }
};

using HashTableKeyword = HashTable<wKeyword, HashTableCell<wKeyword, wKeywrodHash>, wKeywrodHash, HashTableGrower<>>;
*/
#endif // WOLF_HASH_TABLE_HPP