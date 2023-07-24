#pragma once

#include "Exceptions.hpp"

namespace MessageType {
    template <typename MessageType>
    std::string getDescription() {
        throw DescriptionException{ "There isn't that message type!" };
        return "";
    }

    template <BasicMessageType>
    std::string getDescription() {
        return "BasikMessageType";
    }
}
