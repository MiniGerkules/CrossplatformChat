#pragma once

#include <iostream>
#include <mutex>

#include "Logger.hpp"

class ConsoleLogger : public Logger {
private:
    static std::mutex outputMutex_;

public:
    void log(const char* message, const LoggerMessageType type) override {
        auto typeDesc = getMsgTypeDescription(type);

        std::lock_guard lock{ ConsoleLogger::outputMutex_ };
        std::cout << typeDesc << ": " << message << "\n";
    }

private:
    std::string getMsgTypeDescription(const LoggerMessageType type) {
        switch (type) {
            case LoggerMessageType::INFO:
                return "[INFO]";
            case LoggerMessageType::DEBUG:
                return "[DEBUG]";
            case LoggerMessageType::WARNING:
                return "[WARNING]";
            case LoggerMessageType::ERROR:
                return "[ERROR]";
            default:
                return "[UNKNOWN]";
        }
    }
};
