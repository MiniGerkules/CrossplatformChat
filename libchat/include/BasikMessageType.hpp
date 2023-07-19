#pragma once

#include <cstdint>

enum class BasikMessageType: uint8_t {
    CHECK_APP,
    HEARTBEAT,
    ERROR
};
