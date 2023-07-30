#pragma once

#include "../../ResponderChainNode.hpp"

#include "../UniversalMessage.hpp"

class MessageHandler : public ResponderChainNode<MessageHandler> {
public:
    virtual bool canHandle(const UniversalMessage &message) = 0;
    virtual void handle(const UniversalMessage &message) = 0;
};
