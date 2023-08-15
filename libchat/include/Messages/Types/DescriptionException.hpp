#pragma once

#include "BaseException.hpp"

class DescriptionException final : public BaseException {
public:
    DescriptionException(const char *errorMsg) : BaseException{ errorMsg } {
    }
};
