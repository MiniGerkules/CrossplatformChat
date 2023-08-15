#pragma once

#include "Messages/Types/BasicMessageType.hpp"
#include "Messages/Types/MessageTypesFuncs.hpp"

#include "MessageHandler.hpp"
#include "Delegates/BasicMessageHandlerDelegate.hpp"

class BasicMessageHandler : public MessageHandler {
public:
    std::weak_ptr<BasicMessageHandlerDelegate> delegate;

//MARK: - Overrides methods of MessageHandler interface
public:
    bool handle(const UniversalMessage &message) override {
        auto regular = MessageType::convertToTyped<BasicMessageType>(message.header);
        if (!regular.has_value())
            return next_->handle(message);

        if (auto delegatePtr = delegate.lock()) {
            switch (regular.value().typeOption) {
                case BasicMessageType::HEARTBEAT:
                    delegatePtr->messageIsHeartbeat();
                    break;
                case BasicMessageType::CHECK_APP:
                    delegatePtr->messageIsCheck();
                    break;
                case BasicMessageType::ERROR:
                    delegatePtr->messageIsError(MessageType::getTextFrom(message));
                    break;
            }
        }

        return true;
    }

//MARK: - Constructor and methods
public:
    BasicMessageHandler(std::unique_ptr<MessageHandler> next = {})
            : MessageHandler{ std::move(next) } {
    }

    bool isHeartbeat(const UniversalMessage &message) {
        return MessageType::isMessageType<BasicMessageType>(message) &&
               message.header.typeOption == static_cast<uint8_t>(BasicMessageType::HEARTBEAT);
    }

    bool isCheckApp(const UniversalMessage &message) {
        return MessageType::isMessageType<BasicMessageType>(message) &&
               message.header.typeOption == static_cast<uint8_t>(BasicMessageType::CHECK_APP);
    }

    bool isError(const UniversalMessage &message) {
        return MessageType::isMessageType<BasicMessageType>(message) &&
               message.header.typeOption == static_cast<uint8_t>(BasicMessageType::ERROR);
    }
};
