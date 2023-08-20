#pragma once

#include <Messages/UniversalMessage.hpp>

class ClientManagerDelegate {
public:
    virtual ~ClientManagerDelegate() = default;

    virtual void messageFromClient(UniversalMessage message) = 0;
};
