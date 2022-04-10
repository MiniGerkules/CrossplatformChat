#include <cstring>
#include "CMDArgsHelpers.hpp"

LaunchOptions Helpers::processCMDArgs(const int argc, const char* argv[]) {
    if (argc > 2)
        return LaunchOptions::WRONG;
    else if (std::strcmp("--help", argv[1]) == 0)
        return LaunchOptions::NEED_HELP;
    else if (std::strcmp("--client", argv[1]) == 0)
        return LaunchOptions::CLIENT;
    else if (std::strcmp("--server", argv[1]) == 0)
        return LaunchOptions::SERVER;
    else
        return LaunchOptions::CLIENT;
}

void Helpers::outputHelp(std::ostream& stream) {
    stream << "asd" << std::endl;
}
