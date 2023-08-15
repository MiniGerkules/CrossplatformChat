#pragma once

#include "Messages/Message.hpp"
#include "Messages/UniversalMessage.hpp"
#include "Messages/Types/DescriptionException.hpp"

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
bool isMessageType(const UniversalMessageHeader &header) {
    auto desc = getDescription<MessageType>();
    return std::strncmp(header.type, desc.data(), std::size(header.type)) == 0;
}

template <typename MessageType>
bool isMessageType(const UniversalMessage &universal) {
    return isMessageType<MessageType>(universal.header);
}

template <typename MessageType>
std::optional<MessageHeader<MessageType>> convertToTyped(const UniversalMessageHeader &header) {
    if (!isMessageType<MessageType>(header)) return std::nullopt;

    return MessageHeader<MessageType> {
        .typeOption = static_cast<MessageType>(header.typeOption),
        .size = header.size
    };
}

template <typename MessageType>
std::optional<Message<MessageType>> convertToTyped(UniversalMessage universal) {
    if (!isMessageType<MessageType>(universal)) return std::nullopt;

    return Message<MessageType> {
        .header = convertToTyped<MessageType>(universal.header),
        .data = std::move(universal.data)
    };
}

} // MessageType
