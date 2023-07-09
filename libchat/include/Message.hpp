#pragma once

#include <cstdint>
#include <vector>

template <typename IDType>
struct MessageHeader {
    IDType type;
    uint32_t size;
};

template <typename IDType>
struct Message {
    MessageHeader header;
    std::vector<uint8_t> data;
};
