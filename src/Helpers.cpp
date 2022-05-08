#include <cstring>
#include <iostream>

#include "Helpers.hpp"
#include "Message.hpp"
#include "PossibleIDs.hpp"

LaunchOptions Helpers::processCMDArgs(const int argc, const char* argv[]) {
    if (argc == 2) {
        if (std::strcmp("--help", argv[1]) == 0)
            return LaunchOptions::NEED_HELP;
        else if (std::strcmp("--server", argv[1]) == 0)
            return LaunchOptions::SERVER;
        else if (std::strcmp("--client", argv[1]) == 0)
            return LaunchOptions::CLIENT;
    }

    return LaunchOptions::WRONG;
}

void Helpers::outputHelp(std::ostream& stream) {
    stream << "For start as server use flag --server\n";
    stream << "For start as client use flag --client\n";
}

void Helpers::printOptions(std::vector<std::string> options) {
    for (size_t i = 0; i < options.size(); ++i)
        std::cout << i + 1 << ") " << options[i] << "\n";
}

size_t Helpers::chooseOption(size_t min, size_t max) {
    size_t choose;

    while (true) {
        std::cout << "Your choise -- ";
        std::cin >> choose;

        if (!std::cin.fail() && choose >= min && choose <= max) {
            break;
        } else {
            std::cout << "Error: you have entered an invalid option. Try again.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choose;
}

std::string Helpers::getIPOfClient(boost::asio::io_context& ioContext) {
    boost::system::error_code ec;
    std::string ip = "";

    boost::asio::ip::udp::socket socket{ ioContext };
    socket.open(boost::asio::ip::udp::v4(), ec);

    if (!ec) {
        socket.set_option(boost::asio::ip::udp::socket::reuse_address{ true });
        socket.set_option(boost::asio::socket_base::broadcast{ true });

        boost::asio::ip::udp::endpoint endpoint{ boost::asio::ip::address_v4::any(), 60'000 };
        socket.bind(endpoint);

        if (!wait(socket))
            return ip;
        
        MessageHeader<PossibleMessageIDs> msg;
        socket.receive_from(boost::asio::buffer(&msg, sizeof(msg)), endpoint);
        ip = endpoint.address().to_string();

        socket.close(ec);
    }

    return ip;
}

void Helpers::sendMessageToNewClient(boost::asio::io_context& ioContext, std::string ip) {
    boost::system::error_code ec;
    boost::asio::ip::udp::socket socket{ ioContext };
    socket.open(boost::asio::ip::udp::v4(), ec);

    if (!ec) {
        socket.set_option(boost::asio::ip::udp::socket::reuse_address{ true });
        boost::asio::ip::udp::endpoint endpoint;

        if (ip == "") {
            endpoint = boost::asio::ip::udp::endpoint{ boost::asio::ip::address_v4::broadcast(), 60'000 };
            socket.set_option(boost::asio::socket_base::broadcast{ true });
        } else {
            endpoint = boost::asio::ip::udp::endpoint{ boost::asio::ip::make_address(ip), 60'000 };
        }

        MessageHeader<PossibleMessageIDs> msg;
        msg.id = PossibleMessageIDs::findServer;
        socket.send_to(boost::asio::buffer(&msg, sizeof(msg)), endpoint);

        socket.close(ec);
    }
}

bool Helpers::wait(boost::asio::ip::udp::socket& socket) {
    using namespace std::chrono_literals;

    size_t i = 0;
    while (true) {
        if (socket.available() > 0)
            break;
        else if (i >= 20)
            return false;

        ++i;
        std::this_thread::sleep_for(100ms);
    }

    return true;
}
