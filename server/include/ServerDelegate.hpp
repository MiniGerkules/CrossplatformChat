#pragma once

#include <Messages/UniversalMessage.hpp>

class ServerDelegate {
public:
    virtual ~ServerDelegate() = default;

    virtual void messageFromClient(UniversalMessage message) = 0;
};
