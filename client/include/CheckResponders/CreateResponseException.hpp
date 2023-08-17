#pragma once

#include <BaseException.hpp>

class CreateResponseException final : public BaseException {
public:
    CreateResponseException(const char *errorMsg) : BaseException{ errorMsg } {
    }
};
