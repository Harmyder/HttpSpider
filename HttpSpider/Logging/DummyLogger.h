#pragma once

#include "ILogger.h"

namespace Logging
{
    class DummyLogger : public ILogger
    {
    public:
        virtual void AddLine(const std::string &) override {}
        virtual void Add(const char*, ...) override {}
    };
}