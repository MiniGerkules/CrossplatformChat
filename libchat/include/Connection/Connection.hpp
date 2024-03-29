#pragma once

#include <atomic>
#include <memory>
#include <optional>

#include "Delegate.hpp"
#include "Messages/UniversalMessage.hpp"

#include "Delegates/ConnectionDelegate.hpp"

class Connection {
public:
    std::atomic<bool> isAlive = false;
    Delegate<ConnectionDelegate> delegate;

public:
    virtual ~Connection() = default;

    virtual bool open() noexcept = 0;
    virtual void close() noexcept = 0;

    virtual void send(UniversalMessage message) noexcept = 0;
    virtual std::optional<UniversalMessage> read() noexcept = 0;
};
