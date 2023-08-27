#pragma once

#include <memory>
#include <optional>

#include <Delegate.hpp>
#include <Messages/UniversalMessage.hpp>

#include "ReaderDelegate.hpp"

class Reader {
public:
    Delegate<ReaderDelegate> delegate;

public:
    virtual ~Reader() = default;

    virtual std::optional<std::string> read() = 0;
};
