#pragma once

#include <BaseException.hpp>

class ConnectionException : public BaseException {
public:
    ConnectionException(const char *errorMsg) : BaseException{ errorMsg } {
    }
};
