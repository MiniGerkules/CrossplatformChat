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
UniversalMessageHeader convertToUniversal(const MessageHeader<MessageType> &header) {
    UniversalMessageHeader universal = {
        .size = header.size,
        .typeOption = static_cast<uint8_t>(header.typeOption)
    };

    auto desc = getDescription<MessageType>();
    std::strncpy(universal.type, desc.c_str(), std::size(universal.type));

    return universal;
}

template <typename MessageType>
UniversalMessage convertToUniversal(Message<MessageType> message) {
    return UniversalMessage {
        .header = convertToUniversal(message.header),
        .data = std::move(message.data)
    };
}

template <typename MessageType>
bool isMessageType(const UniversalMessage &universal) {
    auto desc = getDescription<MessageType>();
    return std::strncmp(universal.header.type, desc.data(),
                        std::size(universal.header.type)) == 0;
}

} // MessageType
