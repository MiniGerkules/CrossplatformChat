#pragma once

#include "Consts.hpp"

namespace CheckFunctions {

ChatConsts::NumCheckType modFunction(const ChatConsts::NumCheckType x) {
    return ((x << 5) ^ 0xF0F0F0A0A0F0F0F0ull) % 13799;
}

} // CheckFunctions
