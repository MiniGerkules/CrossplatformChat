#pragma once

#include <cstdint>

namespace CheckFunctions {

uint64_t modAppCheckFunction(const uint64_t x) {
    return ((x << 5) ^ 0xF0F0F0A0A0F0F0F0ull) % 13799;
}

} // CheckFunctions
