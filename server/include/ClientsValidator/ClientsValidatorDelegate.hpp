#pragma once

#include <string_view>

#include <Connection/ConnectionManager.hpp>

class ClientsValidatorDelegate {
public:
    virtual ~ClientsValidatorDelegate() = default;

    virtual void clientIsVerified(ConnectionManager client) = 0;
    virtual void clientIsNotVerified(ConnectionManager client, std::string_view error) = 0;
};
