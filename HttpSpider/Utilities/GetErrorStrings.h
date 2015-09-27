#pragma once

namespace Utilities
{
    std::string GetLastErrorString(int errCode);
    std::string GetLastErrorString();

    std::string GetWSALastErrorString(int errCode);
    std::string GetWSALastErrorString();
}