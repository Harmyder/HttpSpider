#pragma once

namespace Utilities
{
    struct UrlComponents;

    class HttpHelper
    {
        HttpHelper(const HttpHelper&) = delete;
        HttpHelper& operator=(const HttpHelper&) = delete;

    public:
        class HttpHelperError : public std::logic_error {
        public:
            explicit HttpHelperError(const std::string& message) : logic_error(message) {}
        };

    public:
        static int GetNeededBufferLength(const UrlComponents& components);
        static void FormGetRequest(const UrlComponents& components, char* buffer, int bufferLength);
    };
}
