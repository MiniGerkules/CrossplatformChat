#pragma once

#include <memory>

#include <ThreadSafe/TSUnorderedMap.hpp>

#include <Delegate.hpp>
#include <Connection/Delegates/ConnectionManagerDelegate.hpp>

#include <Messages/Handlers/RegularMessageHandler.hpp>

#include "ClientsManagerDelegate.hpp"

class ClientsManager : public ConnectionManagerDelegate,
                       public std::enable_shared_from_this<ClientsManager> {
    using Connection_t_ = std::shared_ptr<ConnectionManager>;

public:
    Delegate<ClientsManagerDelegate> delegate;

private:
    std::atomic_size_t lastID_ = 0;
    TSUnorderedMap<Connection_t_, std::pair<const size_t, std::string>> clients_;

//MARK: - Overrides methods of ConnectionManagerDelegate interface
public:
    void ifLostConnection(std::shared_ptr<ConnectionManager> manager,
                          std::string_view errorMsg) override {
        delegate.callIfCan(&ClientsManagerDelegate::clientIsDisconnected, manager, errorMsg);

        manager->close();
        clients_.erase(manager);
    }

    void ifDataIsAvailable(std::shared_ptr<ConnectionManager> manager) override {
        using namespace std::literals;

        auto message = manager->read();
        if (!message.has_value()) return;

        auto connectMsg = MessageType::convertToTyped<ConnectionMessageType>(message.value());
        if (connectMsg.has_value()) {
            if (connectMsg.value().header.typeOption == ConnectionMessageType::DISCONNECT) {
                delegate.callIfCan(&ClientsManagerDelegate::clientIsDisconnected,
                                   manager, "Client is disconnected."sv);
            } else {
                delegate.callIfCan(&ClientsManagerDelegate::clientIsStrange,
                                   manager, "Connection message was received "
                                   "even though the client is already connected "
                                   "and verified."sv);
            }
        } else {
            if (RegularMessageHandler::isSetName(message.value().header)) {
                auto name = MessageType::getTextFrom(message.value());
                clients_[manager].second = name;
            } else {
                delegate.callIfCan(&ClientsManagerDelegate::messageFromClient,
                                   manager, message.value());
            }
        }
    }

//MARK: - Public methods
public:
    void addClient(std::shared_ptr<ConnectionManager> newClient) {
        newClient->delegate = Delegate<ConnectionManagerDelegate>(weak_from_this());
        clients_.insert({ newClient, { lastID_++, "" } });
    }

    void closeAllConnections() {
        for (auto &[client, _] : clients_) {
            client->close();
        }

        clients_.clear();
    }

    std::string getNameOf(std::shared_ptr<ConnectionManager> client) {
        return clients_[client].second;
    }
};
