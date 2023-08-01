#pragma once

#include <BaseException.hpp>

class DisplayerException : public BaseException {
public:
    DisplayerException(const char *errorMsg) : BaseException{ errorMsg } {
    }
};
