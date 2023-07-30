#pragma once

#include <atomic>
#include <memory>
#include <optional>

#include "Delegates/ConnectionDelegate.hpp"
#include "../Messages/UniversalMessage.hpp"

class Connection {
public:
    std::atomic<bool> isAlive = false;
    std::weak_ptr<ConnectionDelegate> delegate;

public:
    Connection(std::weak_ptr<ConnectionDelegate> delegate = std::weak_ptr<ConnectionDelegate>())
            : delegate{ std::move(delegate) } {
    }
    
    virtual ~Connection() = default;

    virtual bool open() noexcept = 0;
    virtual void close() noexcept = 0;
    
    virtual void send(UniversalMessage message) noexcept = 0;
    virtual std::optional<UniversalMessage> read() noexcept = 0;
};
