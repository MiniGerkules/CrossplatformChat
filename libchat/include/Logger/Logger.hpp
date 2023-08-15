#pragma once

#include "LoggerMessageType.hpp"

class Logger {
public:
    virtual ~Logger() = default;

    virtual void log(const char* message, const LoggerMessageType type) = 0;
};
