#pragma once

#include "Messages/Types/RegularMessageType.hpp"

#include "MessageHandler.hpp"
#include "Delegates/RegularMessageHandlerDelegate.hpp"

class RegularMessageHandler : public MessageHandler {
public:
    std::weak_ptr<RegularMessageHandlerDelegate> delegate;

//MARK: - Overrides methods of MessageHandler interface
public:
    bool handle(const UniversalMessage &message) override {
        auto regular = MessageType::convertToTyped<RegularMessageType>(message.header);
        if (!regular.has_value())
            return next_->handle(message);

        if (auto delegatePtr = delegate.lock()) {
            switch (regular.value().typeOption) {
                case RegularMessageType::SET_NAME:
                    delegatePtr->messageSetsName(MessageType::getTextFrom(message));
                    break;
                case RegularMessageType::TEXT_MESSAGE:
                    delegatePtr->messageIsText(MessageType::getTextFrom(message));
                    break;
            }
        }

        return true;
    }

//MARK: - Constructor and methods
public:
    RegularMessageHandler(std::unique_ptr<MessageHandler> next = {})
            : MessageHandler{ std::move(next) } {
    }
};
