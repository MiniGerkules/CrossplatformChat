#pragma once

#include <Messages/Types/RegularMessageType.hpp>
#include <Messages/Handlers/MessageHandler.hpp>
#include <Messages/Handlers/Delegates/RegularMessageHandlerDelegate.hpp>

class RegularMessageHandler : public MessageHandler {
    std::weak_ptr<RegularMessageHandlerDelegate> delegate_;

//MARK: - Overrides methods of MessageHandler interface
public:
    bool handle(const UniversalMessage &message) override {
        auto regular = MessageType::convertToTyped<RegularMessageType>(message.header);
        if (!regular.has_value())
            return next_->handle(message);

        if (auto delegatePtr = delegate_.lock()) {
            switch (regular.value().typeOption) {
                case RegularMessageType::SET_NAME:
                    delegatePtr->messageSetsName(MessageType::getTextFrom(message));
                    break;
                case RegularMessageType::TEXT_MESSAGE:
                    delegatePtr->messageIsText(MessageType::getTextFrom(message));
                    break;
                case RegularMessageType::DISCONNECTED:
                    delegatePtr->messageAboutDisconnection();
                    break;
            }
        }

        return true;
    }

//MARK: - Constructor and methods
public:
    RegularMessageHandler(std::unique_ptr<MessageHandler> next = {},
                          std::weak_ptr<RegularMessageHandlerDelegate> delegate = {})
            : MessageHandler{ std::move(next) }, delegate_{ std::move(delegate) } {
    }
};
