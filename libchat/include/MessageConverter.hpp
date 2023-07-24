#pragma once

#include "Message.hpp"
#include "UniversalMessage.hpp"

#include "MessageTypesDescsGetters.hpp"

namespace MessageType {
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
        std::strcpy(msg.header.type, desc.c_str());

        return msg;
    }
}
