#include "stdafx.h"

#include "LogManager.h"

#include "ILogger.h"

namespace Logging
{
    DEFINE_SINGLETON(LogManager);

    void LogManager::SetLogger(ILogger* logger)
    {
        _ASSERT(logger != _logger);
        _logger = logger;
    }

    ILogger* LogManager::GetLogger()
    {
        return _logger;
    }
}
