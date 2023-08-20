#pragma once

#include <cstdint>
#include <vector>

#include <string>

namespace VectorExtensions {

std::vector<uint8_t> convertToVector(const std::string &str) {
    auto vector = std::vector<uint8_t>(str.length() + 1);
    std::memcpy(vector.data(), str.c_str(), str.length() + 1);

    return vector;
}

template <typename T>
std::vector<uint8_t> convertToVector(const T &value) {
    static_assert(std::is_standard_layout<T>::value, "Type is too complex!");

    auto vector = std::vector<uint8_t>(sizeof(value));
    std::memcpy(vector.data(), &value, sizeof(value));

    return vector;
}

}
