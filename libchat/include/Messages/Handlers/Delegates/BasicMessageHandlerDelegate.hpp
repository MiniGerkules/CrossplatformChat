#pragma once

#include <string_view>

class BasicMessageHandlerDelegate {
public:
    virtual ~BasicMessageHandlerDelegate() = default;

    virtual void messageIsHeartbeat() = 0;
    virtual void messageIsCheck() = 0;
    virtual void messageIsError(const std::string_view error) = 0;
};
