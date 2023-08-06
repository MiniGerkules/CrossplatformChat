#pragma once

#include "MessageHandler.hpp"

#include "../Types/BasicMessageType.hpp"

#include "Delegates/BasicMessageHandlerDelegate.hpp"

class BasicMessageHandler : public MessageHandler {
    std::weak_ptr<BasicMessageHandlerDelegate> delegate_;

//MARK: - Overrides methods of MessageHandler interface
public:
    bool handle(const UniversalMessage &message) override {
        auto regular = MessageType::convertToTyped<BasicMessageType>(message.header);
        if (!regular.has_value())
            return next_->handle(message);

        if (auto delegatePtr = delegate_.lock()) {
            switch (regular.value().typeOption) {
                case BasicMessageType::HEARTBEAT:
                    delegatePtr->messageIsHeartbeat();
                    break;
                case BasicMessageType::CHECK_APP:
                    delegatePtr->messageIsCheck();
                    break;
                case BasicMessageType::ERROR:
                    if (message.header.size == 0)
                        delegatePtr->messageIsError(std::string_view());
                    else
                        delegatePtr->messageIsError(
                            reinterpret_cast<const char *>(message.data.data())
                        );
                    break;
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
