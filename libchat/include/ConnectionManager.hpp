#pragma once

#include <memory>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/asio.hpp>

#include "Connection.hpp"
#include "ConnectionManagerDelegate.hpp"

#include "BasicMessageHandler.hpp"

#include "Message.hpp"
#include "MessageConverter.hpp"

template <typename Executor>
class ConnectionManager : public ConnectionDelegate,
                          public std::enable_shared_from_this<ConnectionManager<Executor>> {
public:
    std::weak_ptr<ConnectionManagerDelegate<Executor>> delegate;
    
private:
    std::string connectionName_;
    std::unique_ptr<Connection> connection_;
    
    BasicMessageHandler handler_;
    TSQueue<UniversalMessage> inMessages_;
                              
    const Executor &executor_;
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
        
        if (!handler_.isHeartbeat(msg.value())) {
            inMessages_.push(std::move(msg.value()));
            
            if (auto delegatePtr = delegate.lock()) {
                delegatePtr->ifDataIsAvailable(*this);
            }
        }
    }
    
    //MARK: - Constructor and methods
public:
    ConnectionManager(std::unique_ptr<Connection> connection,
                      const Executor &executor)
            : connection_{ std::move(connection) }, executor_{ executor } {
        connection_->open();
        connection_->delegate = this->weak_from_this();
    }
    
    bool isAlive() const noexcept {
        return connection_->isAlive;
    }
    
    std::string_view getConnectionName() const & {
        return connectionName_;
    }
                              
    std::optional<UniversalMessage> read() noexcept {
        return inMessages_.pop();
    }
                              
    void send(UniversalMessage message) noexcept {
        connection_->send(std::move(message));
    }
                              
    void close() noexcept {
        connection_->close();
    }
                              
    void startSendingHeartbeat() {
        if (isSendingHeartbeat_.load()) return;

        isSendingHeartbeat_.store(true);
        boost::asio::co_spawn(executor_, asyncSendingHeartbeat(), boost::asio::detached);
    }

    void stopSendingHeartbeat() {
        isSendingHeartbeat_.store(false);
    }

private:
    boost::asio::awaitable<void> asyncSendingHeartbeat() {
        const auto message = MessageType::convertToUniversal(
            Message<BasicMessageType> {
                .header = {
                    .typeOption = BasicMessageType::HEARTBEAT,
                    .size = 0
                },
                .data = {}
            }
        );
        boost::asio::deadline_timer timer{ executor_ };
        
        while (isSendingHeartbeat_) {
            timer.expires_from_now(period_);
            co_await timer.async_wait(boost::asio::use_awaitable);
            
            connection_->send(message);
        }
    }
};
