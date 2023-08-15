#pragma once

#include <cstdint>
#include <vector>

#pragma pack(push, 1)

struct UniversalMessageHeader final {
    char type[30];
    uint8_t typeOption;
    uint32_t size;
};

#pragma pack(pop)

struct UniversalMessage final {
    UniversalMessageHeader header;
    std::vector<uint8_t> data;
};
