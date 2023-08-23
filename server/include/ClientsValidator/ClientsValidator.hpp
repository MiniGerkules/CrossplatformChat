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
    Delegate<ClientsValidatorDelegate> delegate{ {} };

private:
    std::unordered_map<Connection_t_, std::optional<Check_t_>> clients_;

    std::unique_ptr<AppCheckCreater> checkCreater_;
    std::unique_ptr<AppCheckInspector> checkInspector_;

//MARK: - Overrides methods of ConnectionManagerDelegate interface
public:
    void ifLostConnection(ConnectionManager &manager, std::string_view errorMsg) override {
        auto managerPtr = manager.shared_from_this();
        delegate.callIfCan(&ClientsValidatorDelegate::clientIsNotVerified,
                           managerPtr, errorMsg);

        managerPtr->close();
        clients_.erase(managerPtr);
    }

    void ifDataIsAvailable(ConnectionManager &manager) override {
        auto managerPtr = manager.shared_from_this();
        auto message = managerPtr->read();
        if (!message.has_value()) return;

        auto connect = MessageType::convertToTyped<ConnectionMessageType>(message.value());
        if (!connect.has_value()) goto STRANGE_CLIENT;

        // If everything is fine, go out
        if (handleConnectionMessage_(managerPtr, connect.value())) return;

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
        using namespace std::literals;

        manager->close();
        clients_.erase(manager);

        delegate.callIfCan(&ClientsValidatorDelegate::clientIsNotVerified, manager,
                           "Client behaviour is strange. Close connection."sv);
    }

private:
    bool handleConnectionMessage_(Connection_t_ manager, Message<ConnectionMessageType> message) {
        using namespace std::literals;

        switch (message.header.typeOption) {
            case ConnectionMessageType::CONNECT: {
                if (clients_[manager].has_value()) return false;

                auto check = checkCreater_->createCheck();
                manager->send(check);
                clients_[manager] = std::move(check);
                return true;
            }
            case ConnectionMessageType::CHECK_APP:
                if (!clients_[manager].has_value()) return false;

                if (checkInspector_->isCompatibleApp(clients_[manager].value(), message)) {
                    delegate.callIfCan(&ClientsValidatorDelegate::clientIsVerified, manager);
                } else {
                    delegate.callIfCan(&ClientsValidatorDelegate::clientIsNotVerified,
                                       manager, "The check failed."sv);
                }

                clients_.erase(manager);
                return true;
            default:
                return false;
        }
    }
};
