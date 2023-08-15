#pragma once

#include <string_view>

#include "Messages/Message.hpp"
#include "Messages/UniversalMessage.hpp"

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
