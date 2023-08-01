#pragma once

#include "MessageHandler.hpp"
#include "TextHandlerDelegate.hpp"

#include "../Types/MessageTypesGeneralFuncs.hpp"
#include "../Types/BasicMessageType.hpp"

class BasicMessageHandler : public MessageHandler {
    std::weak_ptr<TextHandlerDelegate> delegate_;

//MARK: - Overrides methods of MessageHandler interface
public:
    bool handle(const UniversalMessage &message) override {
        if (!canHandle_(message)) {
            return next_->handle(message);
        } else {
            if (message.header.size != 0) {
                if (auto delegatePtr = delegate_.lock())
                    delegatePtr->handleText(reinterpret_cast<const char *>(message.data.data()));
            }
            
            return true;
        }
    }

//MARK: - Constructor and methods
public:
    BasicMessageHandler(std::unique_ptr<MessageHandler> next = {},
                        std::weak_ptr<TextHandlerDelegate> delegate = {})
            : MessageHandler{ std::move(next) }, delegate_{ std::move(delegate) } {
    }

    bool isHeartbeat(const UniversalMessage &message) {
        return canHandle_(message) &&
               message.header.typeOption == static_cast<uint8_t>(BasicMessageType::HEARTBEAT);
    }

    bool isCheckApp(const UniversalMessage &message) {
        return canHandle_(message) &&
               message.header.typeOption == static_cast<uint8_t>(BasicMessageType::CHECK_APP);
    }

    bool isError(const UniversalMessage &message) {
        return canHandle_(message) &&
               message.header.typeOption == static_cast<uint8_t>(BasicMessageType::ERROR);
    }
    
private:
    bool canHandle_(const UniversalMessage &message) {
        auto desc = MessageType::getDescription<BasicMessageType>();
        return std::strncmp(message.header.type, desc.data(), std::size(message.header.type)) == 0;
    }
};
