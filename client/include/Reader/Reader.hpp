#pragma once

#include <memory>

#include <Messages/UniversalMessage.hpp>

#include "ReaderDelegate.hpp"

class Reader {
public:
    std::weak_ptr<ReaderDelegate> delegate;

public:
    virtual ~Reader() = default;
    virtual UniversalMessage read() = 0;
};
