#pragma once

#include <cstdint>
#include <vector>

#pragma pack(push, 1)

template <typename IDType>
struct MessageHeader {
    static_assert(sizeof(IDType) == sizeof(uint8_t),
                  "IDType doesn't have right size! Must be equal uint8_t!");

    IDType typeOption;      // One of the options of IDType
    uint32_t size;          // Size of this message (including MessageHeader)
};

#pragma pack(pop)

template <typename IDType>
struct Message {
    MessageHeader header;
    std::vector<uint8_t> data;
};
