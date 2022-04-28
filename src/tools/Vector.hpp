#ifndef WOLF_VECTOR_HPP
#define WOLF_VECTOR_HPP

#include <iostream>

template<typename T>
class Vector {
private:
    T* data_;
    size_t capacity_;
    size_t size_;

public:
    Vector() : data_((T*)malloc(sizeof(T))), capacity_(1), size_(0) {
        if (data_ == nullptr) 
            perror("Vector: Failed allocate memory");
    }

    ~Vector() {
        free(data_);
    }

    void PushBack(const T& value) {
        if (size_ == capacity_) {
            capacity_ *= 2;
            T* new_data = (T*) realloc(data_, capacity_ * sizeof(T));

            if (new_data == nullptr) {
                perror("Vector: Failed allocate memory");
                return;
            }

            data_ = new_data;
        }

        data_[size_++] = value;
    }

    void PopBack() {
        if (size_) --size_;
    }

    T& operator[](size_t i) { return data_[i]; }
    const T& operator[](size_t i) const { return data_[i]; }

    inline size_t Size() const { return size_; }

    void Clear() { size_ = 0; }
};


#endif // WOLF_VECTOR_HPP