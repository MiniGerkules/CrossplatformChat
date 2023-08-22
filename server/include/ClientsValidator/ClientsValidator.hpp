#pragma once

#include <unordered_map>

#include <Messages/Handlers/ConnectionMessageHandler.hpp>

#include "ClientsValidatorDelegate.hpp"

#include "CheckCreater/AppCheckCreater.hpp"
#include "CheckInspector/AppCheckInspector.hpp"

class ClientsValidator final : public ConnectionManagerDelegate,
                               public std::enable_shared_from_this<ClientsValidator> {
    using Connection_t_ = std::shared_ptr<ConnectionManager>;
    using Check_t_ = Message<ConnectionMessageType>;

public:
    std::weak_ptr<ClientsValidatorDelegate> delegate;

private:
    std::unordered_map<Connection_t_, std::optional<Check_t_>> clients_;

    std::unique_ptr<AppCheckCreater> checkCreater_;
    std::unique_ptr<AppCheckInspector> checkInspector_;

//MARK: - Overrides methods of ConnectionManagerDelegate interface
public:
    void ifLostConnection(ConnectionManager &manager,
                          const std::string_view errorMsg) override {
        auto managerPtr = manager.shared_from_this();

        clients_.erase(managerPtr);
        managerPtr->close();

        clientIsNotVerified_(managerPtr, errorMsg);
    }

    void ifDataIsAvailable(ConnectionManager &manager) override {
        auto managerPtr = manager.shared_from_this();
        auto message = managerPtr->read();
        if (!message.has_value()) return;

        if (ConnectionMessageHandler::isConnect(message.value().header)) {
            if (clients_[managerPtr].has_value()) goto STRANGE_CLIENT;

            auto check = checkCreater_->createCheck();
            managerPtr->send(check);
            clients_[managerPtr] = std::move(check);
        } else if (ConnectionMessageHandler::isCheckApp(message.value().header)) {
            if (!clients_[managerPtr].has_value()) goto STRANGE_CLIENT;

            auto check = MessageType::convertToTyped<ConnectionMessageType>(message.value());
            if (checkInspector_->isCompatibleApp(clients_[managerPtr].value(), check.value())) {
                clientIsVerified_(managerPtr);
            } else {
                clientIsNotVerified_(managerPtr, "The check failed.");
            }

            clients_.erase(managerPtr);
        } else {
            goto STRANGE_CLIENT;
        }

        return;

    STRANGE_CLIENT:
        strangeClient(managerPtr);
    }

public:
    void addClientToValidate(ConnectionManager newClient) {
        newClient.delegate = weak_from_this();
        clients_.insert({ newClient.shared_from_this(), std::nullopt });
    }

public:
    void strangeClient(Connection_t_ manager) {
        manager->close();
        clients_.erase(manager);

        clientIsNotVerified_(manager, "Client behaviour is strange. Close connection.");
    }

private:
    void clientIsVerified_(Connection_t_ manager) {
        if (auto delegatePtr = delegate.lock()) {
            delegatePtr->clientIsVerified(manager);
        }
    }

    void clientIsNotVerified_(Connection_t_ manager, std::string_view errorMsg) {
        if (auto delegatePtr = delegate.lock()) {
            delegatePtr->clientIsNotVerified(manager, errorMsg);
        }
    }
};
