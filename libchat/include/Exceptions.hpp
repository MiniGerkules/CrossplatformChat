#pragma once

#include <stdexcept>

namespace MessageType {
    class DescriptionException : public std::exception {
    private:
        const char *errorMsg_;
    
    public:
        DescriptionException(const char *errorMsg) : errorMsg_{ errorMsg } {
        }
        
        const char * what() const noexcept override {
            return errorMsg_;
        }
    };
}
