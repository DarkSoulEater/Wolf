#include <assert.h>
#include <stdio.h>
#include <sys\stat.h>

#include "Buffer.hpp"

Buffer::Buffer(size_t size) : data_(nullptr), size_(size) {
    data_ = malloc(size_);
    if (data_ == nullptr) {
        perror("Error: Failed allocate memory for buffer\n");
    }
}

Buffer::Buffer(size_t cnt_elem, size_t sizeof_elem) : data_(nullptr), size_(cnt_elem * sizeof_elem) {
    data_ = malloc(size_);
    if (data_ == nullptr) {
        perror("Error: Failed allocate memory for buffer\n");
    }
}

Buffer::Buffer(const char* file_name) : data_(nullptr), size_(0) {
    FILE* file = nullptr;
    if (fopen_s(&file, file_name, "rb") != 0) {
        perror("Error: Failed to open file\n");
        return;
    }

    struct stat64 file_stat = {};
    fstat64(fileno(file), &file_stat);
    size_ = file_stat.st_size + 1;
    
    data_ = malloc(size_);
    if (data_ == nullptr) {
        perror("Error: Failed allocate memory for buffer\n");
        fclose(file);
        return;
    }

    size_t result = fread(data_, sizeof(char), size_, file);
    if (result != size_ - 1) {
        perror("Error: Incorrect read file\n");
        free(data_);
        fclose(file);
        return;
    }

    fclose(file);
    ((char*) data_)[size_ - 1] = '\0';
}

Buffer::~Buffer() {
    free(data_);
}

bool Buffer::OpenFile(const char* file_name) {
    free(data_);

    FILE* file = nullptr;
    if (fopen_s(&file, file_name, "rb") != 0) {
        perror("Error: Failed to open file\n");
        return false;
    }

    struct stat64 file_stat = {};
    fstat64(fileno(file), &file_stat);
    size_ = file_stat.st_size + 1;
    
    data_ = malloc(size_);
    if (data_ == nullptr) {
        perror("Error: Failed allocate memory for buffer\n");
        fclose(file);
        return false;
    }

    size_t result = fread(data_, sizeof(char), size_, file);
    if (result != size_ - 1) {
        perror("Error: Incorrect read file\n");
        free(data_);
        fclose(file);
        return false;
    }

    fclose(file);
    ((char*) data_)[size_ - 1] = '\0';
    return true;
}