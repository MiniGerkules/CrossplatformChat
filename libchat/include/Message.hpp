#pragma once

#include <cstdint>
#include <vector>

#pragma pack(push, 1)

template <typename IDType>
struct MessageHeader {
    IDType type;
    uint32_t size;
};

#pragma pack(pop)

template <typename IDType>
struct Message {
    MessageHeader header;
    std::vector<uint8_t> data;
};
