#pragma once

#include <stdexcept>

class BaseException : public std::exception {
    const char *errorMsg_;

public:
    BaseException(const char *errorMsg) : errorMsg_{ errorMsg } { }
    virtual ~BaseException() = default;

    const char * what() const noexcept override {
        return errorMsg_;
    }
};
