#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/lockfree/queue.hpp>

#include "TSQueue.hpp"
#include "Connection.hpp"

class TCPConnection : public Connection {
    boost::asio::ip::tcp::socket socket_;
    
    TSQueue<UniversalMessage> inMessages_;

public:
    TCPConnection(boost::asio::ip::tcp::socket socket,
                  std::weak_ptr<ConnectionDelegate> delegate = std::weak_ptr<ConnectionDelegate>())
            : Connection(std::move(delegate)), socket_{ std::move(socket) } {
        isAlive.store(socket_.is_open());
        
        // Start async read from socket
        boost::asio::co_spawn(socket_.get_executor(), asyncMessageReader(), boost::asio::detached);
    }
    
    bool open() override {
        if (socket_.is_open()) return true;
        
        boost::system::error_code ec;
        socket_.open(boost::asio::ip::tcp::v4(), ec);
        const bool res = static_cast<bool>(ec);
        
        isAlive.store(res);
        return res;
    }
    
    void close() override {
        if (!socket_.is_open()) return;
        
        isAlive.store(false);
        socket_.close();
    }
    
    void send(UniversalMessage message) override {
        using boost::asio::detached;
        
        if (!isAlive) return;
        
        co_spawn(socket_.get_executor(), asyncSendData(std::move(message)), detached);
    }
    
    std::optional<UniversalMessage> read() override {
        // There may be some messages when isAlive == false, so we don't check
        // that field
        return inMessages_.pop();
    }
    
private:
    boost::asio::awaitable<void> asyncMessageReader() {
        using namespace boost::asio;
        
        UniversalMessage storage;
        
        try {
            for (;;) {
                co_await async_read(socket_,
                                    buffer(&storage.header, sizeof(storage.header)),
                                    use_awaitable);
                storage.data.resize(storage.header.size);
                co_await async_read(socket_,
                                    buffer(storage.data),
                                    use_awaitable);
                
                inMessages_.push(std::move(storage));
                dataAvailable_();
            }
        } catch (const std::exception &error) {
            lostConnection(error.what());
        }
    }
    
    boost::asio::awaitable<void> asyncSendData(UniversalMessage message) {
        using namespace boost::asio;
        
        try {
            co_await async_write(socket_,
                                 buffer(&message.header, sizeof(message.header)),
                                 use_awaitable);
            co_await async_write(socket_,
                                 buffer(message.data),
                                 use_awaitable);
        } catch (const std::exception &error) {
            lostConnection(error.what());
        }
    }
    
private:
    void lostConnection(std::string errorMsg) {
        if (auto delegatePtr = delegate.lock()) {
            delegatePtr->ifLostConnection(std::move(errorMsg));
        }

        close();
    }
    
    void dataAvailable() {
        if (auto delegatePtr = delegate.lock()) {
            delegatePtr->ifDataIsAvailable();
        }
    }
};
