#ifndef WOLF_HASH_MAP_HPP
#define WOLF_HASH_MAP_HPP

#include <utility>
#include <HashTable.hpp>

template<typename Key, typename Val>
class HashMap {
private:
    struct HashMapKey {
        HashMapKey(std::pair<Key, Val> data) : data_(data) {}

        std::pair<Key, Val> data_;

        bool operator==(const HashMapKey& rhs) const {
            return data_.first == rhs.data_.first;
        }
    };

    struct HashMapHash {
        size_t operator() (const HashMapKey& key) const {
            return key.data_.first.Hash();
        }
    };
    
    friend class HashTable<HashMapKey, HashTableCell<HashMapKey, HashMapHash>, HashMapHash, HashTableGrower<>>;
    using Storage = HashTable<HashMapKey, HashTableCell<HashMapKey, HashMapHash>, HashMapHash, HashTableGrower<>>;
    HashTable<HashMapKey, HashTableCell<HashMapKey, HashMapHash>, HashMapHash, HashTableGrower<>> table_;
public:
using LookupResult = Val*;

    bool Insert(Key key, Val value) {
        auto res = table_.Insert(HashMapKey({key, value}));
        return res.second;
    }

    LookupResult Find(const Key& key) {
        auto res = table_.Find(HashMapKey({key, Val()}));

        if (res == nullptr) return nullptr;
        return &res->key.data_.second;
    }

protected:
    using Cell = HashTableCell<HashMapKey, HashMapHash>;
    // MapIterator
    class MapIterator : public std::iterator<std::input_iterator_tag, std::pair<Key, Val>> {
    public:
        MapIterator(Cell* p) : p(p) {}
        MapIterator(const MapIterator& it) : p(it.p) {}

        bool operator!=(MapIterator const& other) const { return p != other.p; }

        bool operator==(MapIterator const& other) const { return p == other.p; }

        typename MapIterator::reference operator*() const { return (*p).key.data_; };

        MapIterator& operator++() {
            p = p->next_;
            return *this;
        }

    private:
        Cell* p;
    };

public:

    using iterator = MapIterator;
    using const_iterator = MapIterator;

    Cell* Begin() {
        return table_.head_;
    }

    iterator begin() {
        return table_.head_;
    }

    iterator end() {
        return nullptr;
    }

    const_iterator begin() const {
        return table_.Begin();
    }

    const_iterator end() const {
        return nullptr;
    }
};

#endif // WOLF_HASH_MAP_HPP