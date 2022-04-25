#ifndef TOOLS_BUFFER_HPP_
#define TOOLS_BUFFER_HPP_

#include <assert.h>

class Buffer {
private:
    void* data_;
    size_t size_;

public:
    Buffer() : data_(nullptr), size_(0) {}
    Buffer(size_t size); // Size in bytes
    Buffer(size_t cnt_elem, size_t sizeof_elem);
    Buffer(const char* file_name); // Reads all data from the file to the buffer
    ~Buffer();

    bool OpenFile(const char* file_name);
    inline void* GetData() const { return data_; }
    inline size_t GetSize() const { return size_; }

private:
};


#endif // TOOLS_BUFFER_HPP_