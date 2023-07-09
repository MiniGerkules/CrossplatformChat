#pragma once

#include <cstdint>

enum class RegularMessageType : uint8_t {
    HEARTBEAT,
    CHECK,
    SET_NAME,
    MESSAGE
};
