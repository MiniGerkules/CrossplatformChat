#pragma once

#include <cstdint>
#include <string>

#include "MessageTypesFuncs.hpp"

enum class RegularMessageType : uint8_t {
    SET_NAME,
    TEXT_MESSAGE
};

namespace MessageType {
    template <>
    std::string getDescription<RegularMessageType>() {
        return "RegularMessageType";
    }
}
