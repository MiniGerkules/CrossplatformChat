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
    TCPConnection(boost::asio::ip::tcp::socket socket) : socket_{ std::move(socket) } {
        isAlive.store(socket_.is_open());

        // Start async read from socket
        boost::asio::co_spawn(socket_.get_executor(), asyncMessageReader_(), boost::asio::detached);
    }

    bool open() noexcept override {
        if (socket_.is_open()) return true;

        boost::system::error_code ec;
        socket_.open(boost::asio::ip::tcp::v4(), ec);
        const bool res = static_cast<bool>(ec);

        isAlive.store(res);
        return res;
    }

    void close() noexcept override {
        if (!socket_.is_open()) return;

        isAlive.store(false);
        socket_.close();
    }

    void send(UniversalMessage message) noexcept override {
        using boost::asio::detached;

        if (!isAlive) return;

        co_spawn(socket_.get_executor(), asyncSendData_(std::move(message)), detached);
    }

    std::optional<UniversalMessage> read() noexcept override {
        // There may be some messages when isAlive == false, so we don't check
        // that field
        return inMessages_.pop();
    }

private:
    boost::asio::awaitable<void> asyncMessageReader_() noexcept {
        using namespace boost::asio;

        UniversalMessage storage;

        try {
            for (;;) {
                try {
                    co_await async_read(socket_,
                                        buffer(&storage.header, sizeof(storage.header)),
                                        use_awaitable);
                    storage.data.resize(storage.header.size);
                    co_await async_read(socket_,
                                        buffer(storage.data),
                                        use_awaitable);

                    inMessages_.push(std::move(storage));
                    dataAvailable_();
                } catch (const std::bad_alloc &) {      // Ignore vector errors
                } catch (const std::length_error &) {
                }
            }
        } catch (const std::exception &error) {
            lostConnection_(error.what());
        }
    }

    boost::asio::awaitable<void> asyncSendData_(UniversalMessage message) noexcept {
        using namespace boost::asio;

        try {
            co_await async_write(socket_,
                                 buffer(&message.header, sizeof(message.header)),
                                 use_awaitable);
            co_await async_write(socket_,
                                 buffer(message.data),
                                 use_awaitable);
        } catch (const std::exception &error) {
            lostConnection_(error.what());
        }
    }

private:
    void lostConnection_(const std::string_view errorMsg) noexcept {
        if (auto delegatePtr = delegate.lock()) {
            delegatePtr->ifLostConnection(errorMsg);
        }
    }

    void dataAvailable_() noexcept {
        if (auto delegatePtr = delegate.lock()) {
            delegatePtr->ifDataIsAvailable();
        }
    }
};
