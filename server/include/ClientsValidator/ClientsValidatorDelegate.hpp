#pragma once

#include <memory>
#include <string_view>

#include <Connection/ConnectionManager.hpp>

class ClientsValidatorDelegate {
public:
    virtual ~ClientsValidatorDelegate() = default;

    virtual void clientIsVerified(std::shared_ptr<ConnectionManager> manager) = 0;
    virtual void clientIsNotVerified(std::shared_ptr<ConnectionManager> manager,
                                     std::string_view errorMsg) = 0;
};
