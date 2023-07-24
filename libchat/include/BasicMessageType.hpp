#pragma once

#include <cstdint>
#include <string>

enum class BasicMessageType : uint8_t {
    CHECK_APP,
    HEARTBEAT,
    ERROR
};
