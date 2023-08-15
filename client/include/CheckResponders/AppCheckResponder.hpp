#pragma once

#include <Messages/Message.hpp>
#include <Messages/Types/BasicMessageType.hpp>

class AppCheckResponder {
public:
    using Message_t = Message<BasicMessageType>;

public:
    virtual ~AppCheckResponder() = default;

    virtual Message_t createRespond(const Message_t& request) = 0;
};
