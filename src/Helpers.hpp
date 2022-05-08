#pragma once
#include <ostream>
#include <vector>
#include <string>

#include <boost/asio.hpp>

#include "LaunchOptions.hpp"

namespace Helpers {
    LaunchOptions processCMDArgs(const int argc, const char* argv[]);
    void outputHelp(std::ostream& stream);
    void printOptions(std::vector<std::string> options);
    size_t chooseOption(size_t min, size_t max);

    std::string getIPOfClient(boost::asio::io_context& ioContext);
    void sendMessageToNewClient(boost::asio::io_context& ioContext, std::string ip = "");

    bool wait(boost::asio::ip::udp::socket& socket);
}
