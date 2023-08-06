#pragma once

#include <Messages/Types/DescriptionException.hpp>

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
        UniversalMessage universal = {
            .header = {
                .size = message.header.size,
                .typeOption = static_cast<uint8_t>(message.header.typeOption)
            },
            .data = std::move(message.data)
        };

        auto desc = getDescription<MessageType>();
        std::strncpy(universal.header.type, desc.c_str(),
                     std::size(universal.header.type));

        return universal;
    }

    template <typename MessageType>
    bool isMessageType(const UniversalMessage &universal) {
        auto desc = getDescription<MessageType>();
        return std::strncmp(universal.header.type, desc.data(),
                            std::size(universal.header.type)) == 0;
    }
}
