#pragma once

#include <mutex>

#include "LoggerMessageType.hpp"

class ConsoleLogger : public Logger {
private:
    static std::mutex outputMutex_;
    
public:
    void log(const char* message, const LoggerMsgType type) override {
        auto typeDesc = getMsgTypeDescription(type);
        
        std::lock_guard lock{ ConsoleLogger::outputMutex_ };
        std::cout << typeDesc << ": " << message << "\n";
    }
    
private:
    std::string getMsgTypeDescription(const LiggerMsgType type) {
        switch (type) {
            case LoggerMsgType::INFO:
                return "[INFO]";
            case LoggerMsgType::DEBUG:
                return "[DEBUG]";
            case LoggerMsgType::WARNING:
                return "[WARNING]";
            case LoggerMsgType::ERROR:
                return "[ERROR]";
            default:
                return "[UNKNOWN]";
        }
    }
};
