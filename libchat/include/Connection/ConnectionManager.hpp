#pragma once

#include <memory>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/asio.hpp>

#include "TSQueue.hpp"
#include "Messages/Handlers/BasicMessageHandler.hpp"

#include "Messages/Types/MessageTypesFuncs.hpp"

#include "Connection.hpp"
#include "Delegates/ConnectionManagerDelegate.hpp"
#include "Exceptions/ConnectionException.hpp"

class ConnectionManager final : public ConnectionDelegate,
                                public std::enable_shared_from_this<ConnectionManager> {
public:
    std::weak_ptr<ConnectionManagerDelegate> delegate;

private:
    std::unique_ptr<Connection> connection_;

    TSQueue<UniversalMessage> inMessages_;

    boost::posix_time::millisec period_{ 1'000 };
    std::atomic<bool> isSendingHeartbeat_ = false;

//MARK: - Overrides methods of ConnectionDelegate interface
public:
    void ifLostConnection(const std::string_view errorMessage) override {
        if (auto delegatePtr = delegate.lock()) {
            delegatePtr->ifLostConnection(*this, errorMessage);
        }

        connection_->close();
    }

    void ifDataIsAvailable() override {
        auto msg = connection_->read();
        if (!msg.has_value()) return;

        if (!BasicMessageHandler::isHeartbeat(msg.value().header)) {
            inMessages_.push(std::move(msg.value()));

            if (auto delegatePtr = delegate.lock()) {
                delegatePtr->ifDataIsAvailable(*this);
            }
        }
    }

//MARK: - Constructor and methods
public:
    ConnectionManager(std::unique_ptr<Connection> connection)
            : connection_{ std::move(connection) } {
        if (connection == nullptr)
            throw ConnectionException("The connection is unreachable!");

        connection_->delegate = this->weak_from_this();
        connection_->open();
    }

    bool isAlive() const noexcept {
        return connection_->isAlive;
    }

    std::optional<UniversalMessage> read() noexcept {
        return inMessages_.pop();
    }

    void send(UniversalMessage message) noexcept {
        connection_->send(std::move(message));
    }

    template <typename IDType>
    void send(Message<IDType> message) noexcept {
        connection_->send(MessageType::convertToUniversal(std::move(message)));
    }

    void close() noexcept {
        connection_->close();
    }

    template <typename Executor>
    void startSendingHeartbeat(Executor &executor) {
        if (isSendingHeartbeat_.load()) return;

        isSendingHeartbeat_.store(true);
        boost::asio::co_spawn(executor, asyncSendingHeartbeat(executor), boost::asio::detached);
    }

    void stopSendingHeartbeat() {
        isSendingHeartbeat_.store(false);
    }

private:
    template <typename Executor>
    boost::asio::awaitable<void> asyncSendingHeartbeat(Executor &executor) {
        const auto message = MessageType::convertToUniversal(
            Message<BasicMessageType> {
                .header = {
                    .typeOption = BasicMessageType::HEARTBEAT,
                    .size = 0
                }
            }
        );
        boost::asio::deadline_timer timer{ executor };

        while (isSendingHeartbeat_) {
            timer.expires_from_now(period_);
            co_await timer.async_wait(boost::asio::use_awaitable);

            connection_->send(message);
        }
    }
};
