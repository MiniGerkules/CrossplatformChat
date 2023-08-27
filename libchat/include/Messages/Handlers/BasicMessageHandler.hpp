#pragma once

#include "Delegate.hpp"

#include "Messages/Types/BasicMessageType.hpp"
#include "Messages/Types/MessageTypesFuncs.hpp"

#include "MessageHandler.hpp"
#include "Delegates/BasicMessageHandlerDelegate.hpp"

class BasicMessageHandler : public MessageHandler {
public:
    Delegate<BasicMessageHandlerDelegate> delegate;

//MARK: - Static functions
public:
    static bool isHeartbeat(const UniversalMessageHeader &header) {
        return MessageType::isMessageType<BasicMessageType>(header) &&
               header.typeOption == static_cast<uint8_t>(BasicMessageType::HEARTBEAT);
    }

    static bool isError(const UniversalMessageHeader &header) {
        return MessageType::isMessageType<BasicMessageType>(header) &&
               header.typeOption == static_cast<uint8_t>(BasicMessageType::ERROR);
    }

//MARK: - Overrides methods of MessageHandler interface
public:
    bool handle(const UniversalMessage &message) override {
        auto regular = MessageType::convertToTyped<BasicMessageType>(message.header);
        if (!regular.has_value())
            return next_->handle(message);

        switch (regular.value().typeOption) {
            case BasicMessageType::HEARTBEAT:
                delegate.callIfCan(&BasicMessageHandlerDelegate::messageIsHeartbeat);
                break;
            case BasicMessageType::ERROR:
                delegate.callIfCan(&BasicMessageHandlerDelegate::messageIsError,
                                   MessageType::getTextFrom(message));
                break;
            case BasicMessageType::STRANGE_BEHAVIOUR:
                delegate.callIfCan(&BasicMessageHandlerDelegate::messageAboutStrangeBehaviour,
                                   MessageType::getTextFrom(message));
                break;
        }

        return true;
    }

//MARK: - Constructor and methods
public:
    BasicMessageHandler(std::unique_ptr<MessageHandler> next = {})
            : MessageHandler{ std::move(next) } {
    }
};
