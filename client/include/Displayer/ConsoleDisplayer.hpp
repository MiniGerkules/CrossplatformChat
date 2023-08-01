#pragma once

#include <iostream>

#include <TextHandlerDelegate.hpp>

#include "Displayer.hpp"
#include "DisplayerException.hpp"

class ConsoleDisplayer : public TextHandlerDelegate, public Displayer {
//MARK: - Overrides of TextHandlerDelegate interface
public:
    void handleText(const std::string_view text) override {
        std::cout << text << "\n";
    }

//MARK: - Overrides of Displayer abstract base class
public:
    void display(const UniversalMessage &message) override {
        if (handlersChain)
            handlersChain->handle(message);
        else
            throw DisplayerException("Can't handle message! There isn't any handlers!");
    }
};
