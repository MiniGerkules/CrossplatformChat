#pragma once

#include <Messages/UniversalMessage.hpp>

class ServerDelegate {
public:
    virtual void messageFromClient(UniversalMessage message) = 0;
};
