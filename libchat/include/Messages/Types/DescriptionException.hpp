#pragma once

#include "BaseException.hpp"

class DescriptionException : public BaseException {
public:
    DescriptionException(const char *errorMsg) : BaseException{ errorMsg } {
    }
};
