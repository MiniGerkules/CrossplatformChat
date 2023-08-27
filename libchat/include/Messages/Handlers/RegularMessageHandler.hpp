#pragma once

#include "Delegate.hpp"
#include "Messages/Types/RegularMessageType.hpp"

#include "MessageHandler.hpp"
#include "Delegates/RegularMessageHandlerDelegate.hpp"

class RegularMessageHandler : public MessageHandler {
public:
    Delegate<RegularMessageHandlerDelegate> delegate;

//MARK: - Static functions
public:
    static bool isSetName(const UniversalMessageHeader &header) {
        return MessageType::isMessageType<RegularMessageType>(header) &&
               header.typeOption == static_cast<uint8_t>(RegularMessageType::SET_NAME);
    }

//MARK: - Overrides methods of MessageHandler interface
public:
    bool handle(const UniversalMessage &message) override {
        auto regular = MessageType::convertToTyped<RegularMessageType>(message.header);
        if (!regular.has_value())
            return next_->handle(message);

        switch (regular.value().typeOption) {
            case RegularMessageType::SET_NAME:
                delegate.callIfCan(&RegularMessageHandlerDelegate::messageSetsName,
                                   MessageType::getTextFrom(message));
                break;
            case RegularMessageType::TEXT_MESSAGE:
                delegate.callIfCan(&RegularMessageHandlerDelegate::messageIsText,
                                   MessageType::getTextFrom(message));
                break;
        }

        return true;
    }

//MARK: - Constructor and methods
public:
    RegularMessageHandler(std::unique_ptr<MessageHandler> next = {})
            : MessageHandler{ std::move(next) } {
    }
};
