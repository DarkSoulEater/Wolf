#include <iostream>
#include "Buffer.hpp"

int protomain() {
    const char* input_data_file_name = "test/simple_grammar.txt";
    Buffer buf(input_data_file_name);
    char* data = (char*) buf.GetData();
    while (*data) {
        std::cout << *data++;
    }
}