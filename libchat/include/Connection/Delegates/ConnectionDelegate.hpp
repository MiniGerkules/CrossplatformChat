#pragma once

#include <string_view>

class Connection;

class ConnectionDelegate {
public:
    virtual ~ConnectionDelegate() = default;

    virtual void ifLostConnection(const std::string_view what) = 0;
    virtual void ifDataIsAvailable() = 0;
};
