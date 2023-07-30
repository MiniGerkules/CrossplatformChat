#pragma once

#include "Exceptions.hpp"

#include "../Message.hpp"
#include "../UniversalMessage.hpp"

namespace MessageType {
    template <typename MessageType>
    std::string getDescription() {
        throw DescriptionException{ "There isn't that message type!" };
        return "";
    }

    template <typename MessageType>
    UniversalMessage convertToUniversal(Message<MessageType> message) {
        UniversalMessage msg = {
            .header = {
                .size = message.header.size,
                .typeOption = static_cast<uint8_t>(message.header.typeOption)
            },
            .data = std::move(message.data)
        };

        auto desc = getDescription<MessageType>();
        std::strncpy(msg.header.type, desc.c_str(), std::size(msg.header.type));

        return msg;
    }
}
