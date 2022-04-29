#ifndef WOLF_HASH_TABLE_HPP_
#define WOLF_HASH_TABLE_HPP_

#include <assert.h>

#include <cmath>
#include <iostream>
#include <typeinfo>

#define ALWAYS_INLINE _GLIBCXX_ALWAYS_INLINE

/*class HashTableAllocator {
private:
public:
    HashTableAllocator();
    ~HashTableAllocator();

    void* Alloc(size_t size, size_t aligmen) { return _aligned_malloc(); }

    void Free(void* buf, size_t size);

    void* Realloc(void* buf, size_t old_size, size_t new_size);
};*/


template <typename T>
inline bool BitEquals(T && a, T && b) {
    using RealT = std::decay_t<T>;

    if constexpr (std::is_floating_point_v<RealT>)
        return 0 == memcmp(&a, &b, sizeof(RealT));
    else 
        return a == b;
}


template <typename T>
struct Hash {
    size_t operator() (T key) const {
        //return std::hash<T>(key);
        return key;
    }
};


template<typename Key, typename Hash>
struct HashTableCell {
//    Key key;

    //HashTableCell() {}

    HashTableCell(const Key& key_, size_t hash) : key(key_), saved_hash_(hash), is_zero_(false) {}

    const Key& GetKey() const { return key; }

    bool KeyEquals(const Key& key_) {return key_ == key;} //{ return BitEquals(key, key_); }
    bool KeyEquals(const Key& key_, size_t hash_value) {return key == key_;} // { return BitEquals(key, key_); }

    bool IsZero() const { return is_zero_; }
    void SetZero() { is_zero_ = true; }
    void SetBusy() { is_zero_ = false; }

    size_t SetHash(size_t hash) { return saved_hash_ = hash; }
    size_t GetHash() const { return saved_hash_; }

protected:
    Hash hash_;
    size_t saved_hash_;

    bool is_zero_ = true;

public:
    Key key;
    HashTableCell* next_ = nullptr;
    HashTableCell* prev_ = nullptr;
};


template <size_t initial_size_degree = 8>
struct HashTableGrower {
    uint8_t size_degree = initial_size_degree;
    static constexpr auto initial_count = 1ULL << initial_size_degree;

    /// The size of the hash table in the cells.
    size_t BufSize() const               { return 1ULL << size_degree; }

    size_t MaxFill() const               { return 1ULL << (size_degree - 1); }
    size_t Mask() const                  { return BufSize() - 1; }

    /// From the hash value, get the cell number in the hash table.
    size_t Place(size_t x) const         { return x & Mask(); }

    /// The next cell in the collision resolution chain.
    size_t Next(size_t pos) const        { ++pos; return pos & Mask(); }

    /// Whether the hash table is sufficiently full. You need to increase the size of the hash table, or remove something unnecessary from it.
    bool Overflow(size_t elems) const    { return elems > MaxFill(); }

    /// Increase the size of the hash table.
    void IncreaseSize() {
        size_degree += size_degree >= 23 ? 1 : 2;
    }

    void SetBufSize(size_t buf_size_) {
        size_degree = static_cast<size_t>(log2(buf_size_ - 1) + 1);
    }
};

template<
    typename Key,
    typename Cell,
    typename Hash,
    typename Grower
>
class HashTable : protected Hash {
protected:
    friend class HashTableCell<Key, Hash>;

    using HashValue = size_t;
    using Self = HashTable;

    size_t size_ = 0;
    Cell* buf    = nullptr;
    Grower grower_;

#ifdef DB_HASH_MAP_COUNT_COLLISIONS
    mutable size_t collisions = 0;
#endif

    // Find a cell with the same key or an empty cell, starting from the specified position and further along the collision resolution chain.
    size_t ALWAYS_INLINE FindCell(const Key& key, size_t hash_value, size_t place_value) const {
        while (!buf[place_value].IsZero() && !buf[place_value].KeyEquals(key, hash_value)) {
            place_value = grower_.Next(place_value);

#ifdef DB_HASH_MAP_COUNT_COLLISIONS
            ++collisions;
#endif
        }

        return place_value;
    }

    // Find a cell an empty cell, starting from the specified position and further along the collision resolution chain.
    size_t ALWAYS_INLINE FindEmptyCell(size_t place_value) const {
        while (!buf[place_value].IsZero()) {
            place_value = grower_.Next(place_value);

#ifdef DB_HASH_MAP_COUNT_COLLISIONS
            ++collisions;
#endif
        }
        
        return place_value;
    }

    void Alloc(const Grower& new_grower) {
        buf = reinterpret_cast<Cell*>(malloc(new_grower.BufSize() * sizeof(Cell)));
        grower_ = new_grower;

        for (size_t i = 0; i < grower_.BufSize(); ++i) {
            buf[i].SetZero();
        }
    }

    void Free() {
        if (buf) {
            free(buf);
            buf = nullptr;
        }
    }

    void Resize(size_t for_buf_size = 0) {
        std::cout << "Resize\n";
        size_t old_size = grower_.BufSize();
        Grower new_grower = grower_;

        if (for_buf_size) {
            new_grower.SetBufSize(for_buf_size);
            if (new_grower.BufSize() <= old_size) return;
        } else 
            new_grower.IncreaseSize();

        size_t old_buffer_size = GetBufferSizeInSells();
        buf = reinterpret_cast<Cell*>(realloc(buf, new_grower.BufSize() * sizeof(Cell)));

        grower_ = new_grower;
        size_t new_size = grower_.BufSize();
        for (size_t i = old_buffer_size; i < new_size; ++i) {
            buf[i].SetZero();
        }


        size_t i = 0;
        for (; i < old_buffer_size; ++i) {
            if (!buf[i].IsZero()) {
                Reinsert(buf[i], buf[i].GetHash());
            }
        }

        for (; i < new_size && !buf[i].IsZero(); ++i) {
            Reinsert(buf[i], buf[i].GetHash());
        }
    }

    size_t Reinsert(Cell& x, size_t hash_value) {
        size_t place_value = grower_.Place(hash_value);

        // If the element is in its place.
        if (&x == &buf[place_value])
            return place_value;

        place_value = FindCell(x.key, hash_value, place_value);

        // If the item remains in its place in the old collision resolution chain.
        if (!buf[place_value].IsZero())
            return place_value;

        // Copy to a new location and zero the old one.
        x.SetHash(hash_value);
        memcpy(static_cast<void*>(&buf[place_value]), &x, sizeof(x));
        if (x.prev_) x.prev_->next_ = &buf[place_value];
        x.SetZero();

        return place_value;
    }

    void DestroyElements() {
        //assert(0);
        // TODO:
    }

    // Iterator
    class Iterator : public std::iterator<std::input_iterator_tag, Cell> {
        friend class HashTable;
        //friend class Cell;
    private:
        Iterator(Cell* p) : p(p) {}
    public:
        Iterator(const Iterator& it) : p(it.p) {}

        bool operator!=(Iterator const& other) const { return p != other.p; }

        bool operator==(Iterator const& other) const { return p == other.p; }

        typename Iterator::reference operator*() const { return *p; };

        Iterator& operator++() {
            p = p->next_;
            return *this;
        }

    private:
        Cell* p;
    };

public:
    using iterator = Iterator;//<HashTableCell<Key, Hash>>;
    using const_iterator = Iterator;//<const HashTableCell<Key, Hash>>;

    iterator begin() {
        return head_;
    }

    iterator end() {
        return nullptr;
    }

    const_iterator begin() const {
        return head_;
    }

    const_iterator end() const {
        return nullptr;
    }

public:
    using LookupResult = Cell*;
    using ConstLookupResult = const Cell*;

    Cell* head_ = nullptr;
    Cell* tail_ = nullptr;

    size_t hash(const Key& x) const { return Hash::operator()(x); }

    HashTable() {
        Alloc(grower_);
    }

    HashTable(Self && rhs) {
        *this = std::move(rhs);
    }

    ~HashTable() {
        DestroyElements();
        Free();
    }

    /*Self& operator=(Self&& rhs) noexcept {
        DestroyElements();
        free();

        std::swap(buf, rhs.buf);
        std::swap(size_, rhs.size_);
        std::swap(grower_, rhs.grower_);
    }*/

    std::pair<LookupResult, bool> ALWAYS_INLINE Insert(const Key& key) {
        if (GetBufferSizeInSells() < size_ * 2) {
            Resize();
        }

        size_t hash_value = hash(key);
        size_t new_place = FindCell(key, hash_value, grower_.Place(hash_value));
        
        if (buf[new_place].IsZero()) {
            buf[new_place] = Cell(key, hash_value);
            ++size_;

            if (head_ == nullptr) {
                head_ = tail_ = &buf[new_place];
            } else {
                tail_->next_ = &buf[new_place];
                buf[new_place].prev_ = tail_;
                tail_ = &buf[new_place];
            }

            return {&buf[new_place], true};
        }

        return {nullptr, false};
    }

    Cell* Begin() const { return head_; }
    Cell* End() { return tail_; }

    LookupResult ALWAYS_INLINE Find(const Key& key) {
        size_t hash_value = hash(key);
        size_t new_place = FindCell(key, hash_value, grower_.Place(hash_value));

        return buf[new_place].IsZero() ? nullptr : &buf[new_place];
    }

    size_t Size() const {
        return size_;
    }

    bool Empty() const {
        return size_ == 0;
    }

    size_t GetBufferSizeInBytes() const {
        return grower_.BufSize() * sizeof(Cell);
    }

    size_t GetBufferSizeInSells() const {
        return grower_.BufSize();
    }

#ifdef DB_HASH_MAP_COUNT_COLLISIONS
    size_t GetCollisions() const {
        return collisions;
    }
#endif
};


#endif // WOLF_HASH_TABLE_HPP_