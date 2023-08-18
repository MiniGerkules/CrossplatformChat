#pragma once

#include <Messages/Types/ConnectionMessageType.hpp>

#include "MessageHandler.hpp"
#include "Delegates/ConnectionMessageHandlerDelegate.hpp"

class ConnectionMessageHandler : public MessageHandler {
public:
    std::weak_ptr<ConnectionMessageHandlerDelegate> delegate;

//MARK: - Static functions
public:
    static bool isConnect(const UniversalMessageHeader &header) {
        return MessageType::isMessageType<ConnectionMessageType>(header) &&
               header.typeOption == static_cast<uint8_t>(ConnectionMessageType::CONNECT);
    }

    static bool isDisconnect(const UniversalMessageHeader &header) {
        return MessageType::isMessageType<ConnectionMessageType>(header) &&
               header.typeOption == static_cast<uint8_t>(ConnectionMessageType::DISCONNECT);
    }

    static bool isCheckApp(const UniversalMessageHeader &header) {
        return MessageType::isMessageType<ConnectionMessageType>(header) &&
               header.typeOption == static_cast<uint8_t>(ConnectionMessageType::CHECK_APP);
    }

//MARK: - Overrides methods of MessageHandler interface
public:
    bool handle(const UniversalMessage &message) override {
        auto regular = MessageType::convertToTyped<ConnectionMessageType>(message.header);
        if (!regular.has_value())
            return next_->handle(message);

        if (auto delegatePtr = delegate.lock()) {
            switch (regular.value().typeOption) {
                case ConnectionMessageType::CONNECT:
                    delegatePtr->messageToConnect();
                    break;
                case ConnectionMessageType::DISCONNECT:
                    delegatePtr->messageToDisconnect();
                    break;
                case ConnectionMessageType::CHECK_APP:
                    delegatePtr->messageToCheckApp();
                    break;
            }
        }

        return true;
    }

//MARK: - Constructor and methods
public:
    ConnectionMessageHandler(std::unique_ptr<MessageHandler> next = {})
            : MessageHandler{ std::move(next) } {
    }
};
