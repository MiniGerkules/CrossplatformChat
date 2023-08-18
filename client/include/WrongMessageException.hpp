#pragma once

#include <BaseException.hpp>

class WrongMessageException final : public BaseException {
public:
    WrongMessageException(const char *errorMsg) : BaseException{ errorMsg } {
    }
};
