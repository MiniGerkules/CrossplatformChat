#include <iostream>
#include <utility>

#include <Consts.hpp>

#include <Connection/TCPConnection.hpp>
#include <Connection/ConnectionManager.hpp>

#include <Logger/ConsoleLogger.hpp>

#include "Client.hpp"
#include "Reader/ConsoleReader.hpp"
#include "Displayer/ConsoleDisplayer.hpp"
#include "CheckResponders/ModAppCheckResponder.hpp"

#include "WrongMessageException.hpp"

namespace {

#if defined(_POSIX_VERSION)
using Stream = boost::asio::posix::stream_descriptor;
#else
using Stream = boost::asio::windows::stream_handle;
#endif

auto logger = std::make_shared<ConsoleLogger>();

void sendUDPBroadcast(boost::asio::io_context &ioContext, const uint16_t port) {
    using namespace boost::asio;

    ip::udp::socket socket{ ioContext, ip::udp::v4() };

    socket.set_option(ip::udp::socket::reuse_address(true));
    socket.set_option(socket_base::broadcast(true));

    auto message = MessageType::convertToUniversal(
        MessageHeader<ConnectionMessageType> {
            .typeOption = ConnectionMessageType::CONNECT,
            .size = 0
        }
    );
    auto data = buffer(&message, sizeof(message));

    ip::udp::endpoint senderEndpoint(ip::address_v4::broadcast(), port);
    socket.send_to(data, senderEndpoint);
}

std::string getIPOfServer(boost::asio::io_context &ioContext, const uint16_t port,
                          const std::chrono::milliseconds waitFor) {
    using namespace boost::asio;

    ip::udp::endpoint endpoint{ ip::address_v4::any(), port };
    ip::udp::socket socket{ ioContext, endpoint };

    UniversalMessageHeader header;
    auto buffer = boost::asio::buffer(&header, sizeof(header));
    bool receivedData = false;

    auto getMessageFuture = std::async(std::launch::async, [&socket, &buffer, &receivedData]() {
        boost::system::error_code error;
        socket.receive(buffer, 0, error); // 0 -- default value for flags
        receivedData = !error;
    });

    auto timeoutResult = getMessageFuture.wait_for(waitFor);
    if (timeoutResult == std::future_status::timeout || !receivedData) {
        socket.cancel();
        socket.close();

        throw WrongMessageException("Can't get right response from server.");
    }

    if (!ConnectionMessageHandler::isConnect(header))
        throw WrongMessageException("Firts message is not ConnectionMessageType::CONNECT!");

    return endpoint.address().to_string();
}

std::unique_ptr<Connection> createConnection(boost::asio::io_context &ioContext,
                                             const std::string &ip,
                                             const std::string &port,
                                             const std::chrono::milliseconds waitFor) {
    using namespace boost::asio;

    auto endpoints = ip::tcp::resolver{ ioContext }.resolve(ip, port);
    ip::tcp::socket socket{ ioContext, ip::tcp::v4() };
    bool connected = false;

    auto connectFuture = std::async(std::launch::async, [&socket, &endpoints, &connected]() {
        boost::system::error_code error;
        connect(socket, endpoints, error);
        connected = !error;
    });

    auto timeoutResult = connectFuture.wait_for(waitFor);
    if (timeoutResult == std::future_status::timeout || !connected) {
        socket.cancel();
        socket.close();

        return std::unique_ptr<TCPConnection>();
    }

    return std::make_unique<TCPConnection>(std::move(socket));
}

std::unique_ptr<Connection> findServer(boost::asio::io_context &ioContext) {
    using namespace std::chrono_literals;

    try {
        logger->log("Sending broadcast to find the server.", LoggerMessageType::INFO);
        sendUDPBroadcast(ioContext, ChatConsts::getIPPort);

        logger->log("Trying to get message from the server.", LoggerMessageType::INFO);
        auto ip = getIPOfServer(ioContext, ChatConsts::getIPPort, 1'000ms);

        logger->log("Trying to connect to the server.", LoggerMessageType::INFO);
        return createConnection(ioContext, ip, std::to_string(ChatConsts::mainPort), 1'000ms);
    } catch (const std::exception &error) {
        logger->log("Can't find server with error:", LoggerMessageType::ERROR);
        logger->log(error.what(), LoggerMessageType::ERROR);

        return std::unique_ptr<TCPConnection>();
    }
}

}

int main() {
    using namespace boost::asio;

    io_context ioContext;
    Stream stream{ ioContext, STDIN_FILENO };
    ConsoleReader reader{ ioContext, std::move(stream) };
    ConsoleDisplayer displayer;

    auto connection = findServer(ioContext);
    if (connection == nullptr) {
        logger->log("Can't find server!", LoggerMessageType::ERROR);
        return 0;
    }

    Client client{
        ioContext,
        std::move(connection),
        std::make_unique<ConsoleReader<io_context, Stream>>(ioContext, std::move(stream)),
        std::make_unique<ConsoleDisplayer>(),
        std::make_unique<ModAppCheckResponder>()
    };
    client.logger = Delegate<Logger>(logger);

    signal_set signals(ioContext, SIGINT, SIGTERM);
    signals.async_wait([&client](auto, auto) { client.stop(); });

    client.run();

    return 0;
}
