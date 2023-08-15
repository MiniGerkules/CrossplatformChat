#pragma once

#include <boost/asio.hpp>

#include <TSQueue.hpp>

#include "Reader.hpp"

template <typename Executor, typename Stream>
class ConsoleReader final : public Reader {
    Stream stream_;
    TSQueue<std::string> inputs_;

public:
    ConsoleReader(Executor &executor, Stream stream) : stream_{ std::move(stream) } {
        // Start async reading from console
        boost::asio::co_spawn(executor, asyncInputsReader(), boost::asio::detached);
    }

    std::optional<std::string> read() override {
        return inputs_.pop();
    }

private:
    boost::asio::awaitable<void> asyncInputsReader() {
        using namespace boost::asio;

        char data[1024];

        for (;;) {
            size_t n = co_await stream_.async_read_some(buffer(data), use_awaitable);
            inputs_.push(std::string(data, n));
        }
    }
};
