#pragma once

#include "MessageHandler.hpp"
#include "TextHandlerDelegate.hpp"

#include "../Types/MessageTypesGeneralFuncs.hpp"
#include "../Types/BasicMessageType.hpp"

class BasicMessageHandler : public MessageHandler {
    std::weak_ptr<TextHandlerDelegate> delegate;

//MARK: - Overrides methods of MessageHandler interface
public:
    bool canHandle(const UniversalMessage &message) override {
        auto desc = MessageType::getDescription<BasicMessageType>();
        return message.header.type == desc;
    }

    void handle(const UniversalMessage &message) override {
        if (!canHandle(message)) return;

        if (message.header.size != 0) {
            if (auto delegatePtr = delegate.lock())
                delegatePtr->handleText(reinterpret_cast<const char *>(message.data.data()));
        }
    }

//MARK: - Constructor and methods
public:
    BasicMessageHandler(std::weak_ptr<TextHandlerDelegate> delegate = {})
            : delegate{ std::move(delegate) } {
    }

    bool isHeartbeat(const UniversalMessage &message) {
        return canHandle(message) &&
               message.header.typeOption == static_cast<uint8_t>(BasicMessageType::HEARTBEAT);
    }

    bool isCheckApp(const UniversalMessage &message) {
        return canHandle(message) &&
               message.header.typeOption == static_cast<uint8_t>(BasicMessageType::CHECK_APP);
    }

    bool isError(const UniversalMessage &message) {
        return canHandle(message) &&
               message.header.typeOption == static_cast<uint8_t>(BasicMessageType::ERROR);
    }
};
