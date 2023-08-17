#pragma once

#include <cstdint>
#include <string>

#include "MessageTypesFuncs.hpp"

enum class ConnectionMessageType : uint8_t {
    CONNECT,
    DISCONNECT,
    CHECK_APP
};

namespace MessageType {
    template <>
    std::string getDescription<ConnectionMessageType>() {
        return "ConnectionMessageType";
    }
}
