#pragma once

#include <memory>
#include <unordered_set>

#include <Delegate.hpp>
#include <Connection/Delegates/ConnectionManagerDelegate.hpp>

#include "ClientsManagerDelegate.hpp"

class ClientsManager : public ConnectionManagerDelegate,
                       public std::enable_shared_from_this<ClientsManager> {
public:
    Delegate<ClientsManagerDelegate> delegate{ {} };

private:
    std::unordered_set<std::shared_ptr<ConnectionManager>> clients_;

//MARK: - Overrides methods of ConnectionManagerDelegate interface
public:
    void ifLostConnection(ConnectionManager &manager, std::string_view errorMsg) override {
        auto managerPtr = manager.shared_from_this();
        delegate.callIfCan(&ClientsManagerDelegate::clientIsDisconnected, managerPtr, errorMsg);

        managerPtr->close();
        clients_.erase(managerPtr);
    }

    void ifDataIsAvailable(ConnectionManager &manager) override {
        using namespace std::literals;

        auto menagerPtr = manager.shared_from_this();
        auto message = menagerPtr->read();
        if (!message.has_value()) return;

        auto connectMsg = MessageType::convertToTyped<ConnectionMessageType>(message.value());
        if (connectMsg.has_value()) {
            if (connectMsg.value().header.typeOption == ConnectionMessageType::DISCONNECT) {
                delegate.callIfCan(&ClientsManagerDelegate::clientIsDisconnected,
                                   menagerPtr, "Client is disconnected."sv);
            } else {
                delegate.callIfCan(&ClientsManagerDelegate::clientIsStrange,
                                   menagerPtr, "Connection message was received "
                                   "even though the client is already connected "
                                   "and verified."sv);
            }
        } else {
            delegate.callIfCan(&ClientsManagerDelegate::messageFromClient,
                               menagerPtr, message.value());
        }
    }

//MARK: - Public methods
public:
    void addClient(std::shared_ptr<ConnectionManager> newClient) {
        newClient->delegate = weak_from_this();
        clients_.insert(newClient);
    }

    void closeAllConnections() {
        for (auto &client : clients_) {
            client->close();
        }

        clients_.clear();
    }
};
