#pragma once

#include <optional>
#include <string_view>

#include "Messages/Message.hpp"
#include "Messages/UniversalMessage.hpp"

#include "DescriptionException.hpp"

namespace MessageType {

namespace Details {

std::string_view getTextFromVector(const std::vector<uint8_t> &data) {
    if (data.size() != 0)
        return reinterpret_cast<const char *>(data.data());
    else
        return std::string_view();
}

} // Details

/// The function returns text from `message`.
///
/// Use only for lvalue messages! std::string\_view **doesn't store** data!
/// - Parameter message: The message whose text you want to receive
/// - Returns: Text from the `message`
std::string_view getTextFrom(const UniversalMessage &message) {
    return Details::getTextFromVector(message.data);
}

template <typename IDType>
/// The function returns text from `message`.
///
/// Use only for lvalue messages! std::string\_view **doesn't store** data!
/// - Parameter message: The message whose text you want to receive
/// - Returns: Text from the `message`
std::string_view getTextFrom(const Message<IDType> &message) {
    return Details::getTextFromVector(message.data);
}

} // MessageType


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

} // MessageType


namespace MessageType {

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
