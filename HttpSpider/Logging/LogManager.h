#pragma once

#include "ILogger.h"
#include "SDK/Singleton.h"

#define LOG_ADD_LINE(line) Logging::LogManager::GetInstance().GetLogger()->AddLine(line)
#define LOG_ADD_FMT(fmt, ...) Logging::LogManager::GetInstance().GetLogger()->Add((fmt), ##__VA_ARGS__)

namespace Logging
{
    class LogManager
    {
        DECLARE_CLASS_SINGLETON(LogManager);

    public:
        void SetLogger(ILogger* logger);
        ILogger* GetLogger();

    private:
        ILogger* _logger = NULL;
    };
}