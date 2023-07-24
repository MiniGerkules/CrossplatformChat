#pragma once

#include <string_view>

class ConnectionManager;

class ConnectionManagerDelegate {
public:
    virtual void ifLostConnection(const ConnectionManager &manager,
                                  const std::string_view errorMsg) = 0;
    virtual void ifDataIsAvailable(const ConnectionManager &manager) = 0;
};
