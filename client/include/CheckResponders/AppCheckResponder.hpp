#pragma once

#include <Messages/Message.hpp>
#include <Messages/Types/ConnectionMessageType.hpp>

class AppCheckResponder {
public:
    using Message_t = Message<ConnectionMessageType>;

public:
    virtual ~AppCheckResponder() = default;

    virtual Message_t createResponse(const Message_t& request) = 0;
};
