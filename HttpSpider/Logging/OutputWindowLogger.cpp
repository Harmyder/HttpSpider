#include "stdafx.h"

#include "OutputWindowLogger.h"

#include <cstdarg>

using namespace std;

namespace Logging
{
    void OutputWindowLogger::AddLine(const string &message)
    {
        OutputDebugString(message.c_str());
        OutputDebugString("\n");
    }
    void OutputWindowLogger::Add(const char* formatstring, ...)
    {
        va_list args;
        va_start(args, formatstring);

        char buf[1024];
        memset(buf, 0, sizeof(buf));
        vsnprintf_s(buf, _countof(buf), _TRUNCATE, formatstring, args);

        OutputDebugString(buf);

        va_end(args);
    }
}