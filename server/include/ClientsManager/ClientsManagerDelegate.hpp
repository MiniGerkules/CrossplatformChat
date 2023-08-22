#pragma once

#include <memory>
#include <string_view>

#include <Messages/UniversalMessage.hpp>
#include <Connection/ConnectionManager.hpp>

class ClientsManagerDelegate {
public:
    virtual ~ClientsManagerDelegate() = default;

    virtual void messageFromClient(std::shared_ptr<ConnectionManager> manager,
                                   UniversalMessage message) = 0;
    virtual void clientIsDisconnected(std::shared_ptr<ConnectionManager> manager,
                                      std::string_view reason) = 0;
};
