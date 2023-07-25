#pragma once

#include "LoggerMessageType.hpp"

class Logger {
public:
    virtual void log(const char* message, const LoggerMsgType type) = 0;
};
