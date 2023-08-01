#pragma once

#include <memory>

#include <Messages/UniversalMessage.hpp>
#include <Messages/Handlers/MessageHandler.hpp>

class Displayer {
public:
    std::shared_ptr<MessageHandler> handlersChain;

public:
    virtual ~Displayer() = default;
    virtual void display(const UniversalMessage &message) = 0;
};
