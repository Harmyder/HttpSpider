#pragma once

namespace Utilities
{
    struct HttpResponseComponents
    {
        HttpResponseComponents();

        int status;
    };

    class HttpResponseParser
    {
        HttpResponseParser(const HttpResponseParser&) = delete;
        HttpResponseParser& operator=(const HttpResponseParser&) = delete;

    public:
        class HttpResponseParserError : public std::logic_error {
        public:
            explicit HttpResponseParserError(const std::string& message) : logic_error(message) {}
        };

    public:
        HttpResponseParser() : _sealed(false) {}

        int ParseFromBuffer(const char *buffer);
        void AddLine(const std::string& line);

        const HttpResponseComponents& Components() { return _components; }
        bool Sealed() { return _sealed; }

    private:
        void Seal();
        bool IsLineEmpty(const std::string& line);

    private:
        HttpResponseComponents _components;
        bool _sealed;
    };
}