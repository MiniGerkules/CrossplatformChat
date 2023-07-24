#pragma once

#include <string_view>

class TextHandlerDelegate {
public:
    virtual void handleText(const std::string_view text) = 0;
};
