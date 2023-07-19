#pragma once

#include <cstdint>

enum class RegularMessageType : uint8_t {
    SET_NAME,
    TEXT_MESSAGE,
    DISCONNECTED
};
