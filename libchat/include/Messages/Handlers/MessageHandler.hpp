#pragma once

#include "../UniversalMessage.hpp"

class MessageHandler {
public:
    virtual bool canHandle(const UniversalMessage &message) = 0;
    virtual void handle(const UniversalMessage &message) = 0;
};
