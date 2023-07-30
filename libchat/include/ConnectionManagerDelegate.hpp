#pragma once

#include <string_view>

template <typename Executor>
class ConnectionManager;

template <typename Executor>
class ConnectionManagerDelegate {
public:
    virtual void ifLostConnection(const ConnectionManager<Executor> &manager,
                                  const std::string_view errorMsg) = 0;
    virtual void ifDataIsAvailable(const ConnectionManager<Executor> &manager) = 0;
};
