#ifndef C_STRING_HPP
#define C_STRING_HPP

#include <cstring>
#include <stdexcept>

inline char* copy_c_string(const char* text) {
    if (text == nullptr) throw std::invalid_argument("Value must not be null");
    const std::size_t size = std::strlen(text) + 1;
    char* copy = new char[size];
    std::memcpy(copy, text, size);
    return copy;
}

#endif

