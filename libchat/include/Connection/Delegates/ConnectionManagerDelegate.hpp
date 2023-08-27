#pragma once

#include <string_view>

class ConnectionManager;

class ConnectionManagerDelegate {
public:
    virtual ~ConnectionManagerDelegate() = default;

    virtual void ifLostConnection(std::shared_ptr<ConnectionManager> manager,
                                  std::string_view errorMsg) = 0;
    virtual void ifDataIsAvailable(std::shared_ptr<ConnectionManager> manager) = 0;
};
