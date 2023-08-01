#pragma once

#include "../../ResponderChainNode.hpp"

#include "../UniversalMessage.hpp"

class MessageHandler : public ResponderChainNode<MessageHandler> {
public:
    virtual bool handle(const UniversalMessage &message) = 0;

private:
    virtual bool canHandle_(const UniversalMessage &message) = 0;
};
