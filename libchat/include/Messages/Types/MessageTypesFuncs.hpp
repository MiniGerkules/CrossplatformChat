#pragma once

#include <string_view>

#include <Messages/UniversalMessage.hpp>

namespace MessageType {

/// The function returns text from `message`.
///
/// Use only for lvalue messages! std::string\_view **doesn't store** data!
/// - Parameter message: the message whose text you want to receive
/// - Returns: text from the `message`
std::string_view getTextFrom(const UniversalMessage &message) {
    if (message.header.size != 0)
        return reinterpret_cast<const char *>(message.data.data());
    else
        return std::string_view();
}

} // MessageType
