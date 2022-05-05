#pragma once
#include <ostream>
#include <vector>

#include "LaunchOptions.hpp"

namespace Helpers {
    LaunchOptions processCMDArgs(const int argc, const char* argv[]);
    void outputHelp(std::ostream& stream);
    void printOptions(std::vector<std::string> options);
    size_t chooseOption(size_t min, size_t max);
}
