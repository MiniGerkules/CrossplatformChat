#pragma once

#include <memory>
#include <optional>

#include <Messages/UniversalMessage.hpp>

#include "ReaderDelegate.hpp"

class Reader {
public:
    std::weak_ptr<ReaderDelegate> delegate;

public:
    virtual ~Reader() = default;

    virtual std::optional<std::string> read() = 0;
};
