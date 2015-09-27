#include "stdafx.h"

#include "IostreamLogger.h"

#include <cstdarg>
#include <stdlib.h>

#include <iostream>
using namespace std;

namespace Logging
{
    void IostreamLogger::AddLine(const string &message)
    {
        cout << message << endl;
    }
    void IostreamLogger::Add(const char* formatstring, ...)
    {
        va_list args;
        va_start(args, formatstring);

        char buf[1024];
        memset(buf, 0, sizeof(buf));
        vsnprintf_s(buf, _countof(buf), _TRUNCATE, formatstring, args);

        cout << buf;

        va_end(args);
    }
}