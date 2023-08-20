#pragma once

#include <charconv>

#include <VectorExtensions.hpp>

#include <Messages/Types/MessageTypesFuncs.hpp>
#include <Functions/ModFunction.hpp>

#include "AppCheckResponder.hpp"
#include "CreateResponseException.hpp"

class ModAppCheckResponder final : public AppCheckResponder {
public:
    Message_t createResponse(const Message_t& request) override {
        auto text = MessageType::getTextFrom(request);
        uint64_t requestNum;
        auto [_, ec] = std::from_chars(text.data(), text.data() + text.size(), requestNum);

        if (ec != std::errc())
            throw CreateResponseException("Can't convert message body to number!");

        auto responseNum = CheckFunctions::modFunction(requestNum);
        Message_t message = {
            .header = {
                .typeOption = ConnectionMessageType::CHECK_APP,
                .size = sizeof(responseNum)
            },
            .data = VectorExtensions::convertToVector(responseNum)
        };

        return message;
    }
};
