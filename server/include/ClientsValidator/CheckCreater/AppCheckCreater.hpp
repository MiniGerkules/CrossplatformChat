#pragma once

#include <Messages/Message.hpp>
#include <Messages/Types/ConnectionMessageType.hpp>

class AppCheckCreater {
public:
    virtual ~AppCheckCreater() = default;

    virtual Message<ConnectionMessageType> createCheck() = 0;
};
