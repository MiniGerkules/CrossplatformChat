#pragma once

#include <string_view>

class ConnectionManager;

class ConnectionManagerDelegate {
public:
    virtual ~ConnectionManagerDelegate() = default;

    virtual void ifLostConnection(ConnectionManager &manager,
                                  const std::string_view errorMsg) = 0;
    virtual void ifDataIsAvailable(ConnectionManager &manager) = 0;
};
