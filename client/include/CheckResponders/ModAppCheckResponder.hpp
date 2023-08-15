#pragma once

#include <charconv>

#include <Messages/Types/MessageTypesFuncs.hpp>
#include <Functions/ModFunction.hpp>

#include "AppCheckResponder.hpp"

class ModAppCheckResponder final : public AppCheckResponder {
public:
    Message_t createResponse(const Message_t& request) override {
        auto text = MessageType::getTextFrom(request);
        uint64_t requestNum;
        auto [_, ec] = std::from_chars(text.data(), text.data() + text.size(), requestNum);

        if (ec != std::errc())
            throw std::exception();

        uint64_t responseNum = CheckFunctions::modFunction(requestNum);
        Message_t message = {
            .header = {
                .typeOption = BasicMessageType::CHECK_APP,
                .size = sizeof(responseNum)
            },
            .data = std::vector<uint8_t>(sizeof(responseNum))
        };

        std::memcpy(message.data.data(), &responseNum, sizeof(responseNum));

        return message;
    }
};
