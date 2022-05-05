#include <boost/asio.hpp>
#include <iostream>
#include <memory>

#include "Helpers.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "PossibleIDs.hpp"

int main(int argc, const char* argv[]) {
    std::unique_ptr<Runnable> runnable;

    LaunchOptions launchOption = Helpers::processCMDArgs(argc, argv);
    switch (launchOption) {
    case LaunchOptions::WRONG:
        std::cout << "Wrong arguments of command line!" << std::endl;
        break;
    case LaunchOptions::NEED_HELP:
        Helpers::outputHelp(std::cout);
        break;
    case LaunchOptions::CLIENT:
        runnable = std::make_unique<Client>();
        break;
    case LaunchOptions::SERVER:
        runnable = std::make_unique<Server<PossibleMessageIDs>>();
        break;
    }

    if (runnable != nullptr)
        runnable->run();

    return 0;
}
