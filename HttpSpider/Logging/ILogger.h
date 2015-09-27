#pragma once

namespace Logging
{
    class ILogger
    {
    public:
        virtual ~ILogger() { }
        virtual void AddLine(const std::string &message) = 0;
        virtual void Add(const char* formatstring, ...) = 0;
    };
}