#include <cstring>
#include <iostream>
#include "Helpers.hpp"

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
