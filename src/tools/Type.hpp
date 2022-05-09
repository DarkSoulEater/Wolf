#ifndef WOLF_TYPE_HPP
#define WOLF_TYPE_HPP

#include <string.h>
#include <immintrin.h>

struct MapString32{
    char str[33] = {};

    MapString32() = default;

    MapString32(const char* str_) {
        strcpy_s(str, str_);
    }

    bool operator==(const MapString32& rhs) const {
        return strcmp(str, rhs.str) == 0;
    }

    bool operator<(const MapString32& rhs) const {
        return strcmp(str, rhs.str) < 0;
    }

    bool operator>(const MapString32& rhs) const {
        return strcmp(str, rhs.str) > 0;
    }

    char& operator[](size_t i) {
        return str[i];
    }

    size_t Hash() const {
        unsigned __int64 hash = 0;
        for (size_t i = 0; i < 4; ++i) {
            if (*((const unsigned __int64*)str + i) == 0ull) break;
            hash = _mm_crc32_u64(hash, *((const unsigned __int64*)str + i));
        }
        return (size_t)hash;
    }
};


#endif // WOLF_TYPE_HPP