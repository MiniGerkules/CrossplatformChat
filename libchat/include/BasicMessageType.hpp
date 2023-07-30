#pragma once

#include <cstdint>
#include <string>

#include "MessageTypesGeneralFuncs.hpp"

enum class BasicMessageType : uint8_t {
    CHECK_APP,
    HEARTBEAT,
    ERROR
};

namespace MessageType {
    template <BasicMessageType>
    std::string getDescription() {
        return "BasicMessageType";
    }
}
