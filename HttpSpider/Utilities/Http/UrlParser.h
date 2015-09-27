#pragma once

namespace Utilities
{
    struct UrlComponents
    {
        std::string scheme;
        std::string username;
        std::string password;
        std::string host;
        std::string port;
        std::string path;
        std::string query;
        std::string ref;
    };

    UrlComponents ParseUrl(const std::string& url);
}
