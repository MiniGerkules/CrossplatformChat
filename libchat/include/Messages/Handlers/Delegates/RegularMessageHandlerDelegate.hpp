#pragma once

#include <string_view>

class RegularMessageHandlerDelegate {
public:
    virtual ~RegularMessageHandlerDelegate() = default;

    virtual void messageSetsName(const std::string_view name) = 0;
    virtual void messageIsText(const std::string_view text) = 0;
};
