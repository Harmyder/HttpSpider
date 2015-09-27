#pragma once

#include "ILogger.h"

namespace Logging
{
    class IostreamLogger : public ILogger
    {
    public:
        virtual void AddLine(const std::string &message) override;
        virtual void Add(const char* formatstring, ...) override;
    };
}