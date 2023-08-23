#pragma once

#include <string_view>

class BasicMessageHandlerDelegate {
public:
    virtual ~BasicMessageHandlerDelegate() = default;

    virtual void messageIsHeartbeat() = 0;
    virtual void messageIsError(std::string_view error) = 0;
    virtual void messageAboutStrangeBehaviour(std::string_view info) = 0;
};
