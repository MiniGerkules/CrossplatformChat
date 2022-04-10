#pragma once
#include <ostream>
#include "LaunchOptions.hpp"

namespace Helpers {
    LaunchOptions processCMDArgs(const int argc, const char* argv[]);
    void outputHelp(std::ostream& stream);
}
