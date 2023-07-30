#pragma once

#include <memory>

template <typename T>
class ResponderChainNode {
public:
    virtual std::shared_ptr<T> next() = 0;
};
