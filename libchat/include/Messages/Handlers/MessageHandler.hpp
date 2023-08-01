#pragma once

#include "../../ResponderChainNode.hpp"

#include "../UniversalMessage.hpp"

class MessageHandler {
protected:
    std::unique_ptr<MessageHandler> next_;
    
public:
    MessageHandler(std::unique_ptr<MessageHandler> next) : next_{ std::move(next) } { }
    virtual ~MessageHandler() = default;
    
    virtual bool handle(const UniversalMessage &message) = 0;
};
