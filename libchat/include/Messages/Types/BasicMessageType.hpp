#pragma once

#include <cstdint>
#include <string>

#include "MessageTypesFuncs.hpp"

enum class BasicMessageType : uint8_t {
    CHECK_APP,
    HEARTBEAT,
    ERROR
};

namespace MessageType {
    template <>
    std::string getDescription<BasicMessageType>() {
        return "BasicMessageType";
    }
}
