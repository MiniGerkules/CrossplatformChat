#pragma once

#include <cstdint>
#include <string>

#include "MessageTypesGeneralFuncs.hpp"

enum class RegularMessageType : uint8_t {
    SET_NAME,
    TEXT_MESSAGE,
    DISCONNECTED
};

namespace MessageType {
    template <>
    std::string getDescription<RegularMessageType>() {
        return "RegularMessageType";
    }
}