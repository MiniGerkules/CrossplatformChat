#pragma once

#include "MessageHandler.hpp"

#include "../Types/BasicMessageType.hpp"

#include "Delegates/BasicMessageHandlerDelegate.hpp"

class BasicMessageHandler : public MessageHandler {
    std::weak_ptr<BasicMessageHandlerDelegate> delegate_;

//MARK: - Overrides methods of MessageHandler interface
public:
    bool handle(const UniversalMessage &message) override {
        if (!MessageType::isMessageType<BasicMessageType>(message)) {
            return next_->handle(message);
        }

        if (message.header.size != 0) {
            if (auto delegatePtr = delegate_.lock()) {
                if (isHeartbeat(message))
                    delegatePtr->messageIsHeartbeat();
                else if (isCheckApp(message))
                    delegatePtr->messageIsCheck();
                else
                    delegatePtr->messageIsError(reinterpret_cast<const char *>(message.data.data()));
            }
        }

        return true;
    }

//MARK: - Constructor and methods
public:
    BasicMessageHandler(std::unique_ptr<MessageHandler> next = {},
                        std::weak_ptr<BasicMessageHandlerDelegate> delegate = {})
            : MessageHandler{ std::move(next) }, delegate_{ std::move(delegate) } {
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
