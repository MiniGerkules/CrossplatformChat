#pragma once

#include <iostream>

#include "Displayer.hpp"
#include "DisplayerException.hpp"

class ConsoleDisplayer final : public Displayer {
//MARK: - Overrides of Displayer abstract base class
public:
    void display(const UniversalMessage &message) override {
        if (handlersChain)
            handlersChain->handle(message);
        else
            throw DisplayerException("Can't handle message! There isn't any handlers!");
    }

    void display(const std::string_view &message) override {
        std::cout << message << '\n';
    }
};
