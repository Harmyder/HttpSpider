#include "stdafx.h"

#include "Utilities/DebugPrint.h"
#include <stdio.h>
#include <stdlib.h>

namespace Utilities
{
    void DebugPrintf(const char* formatstring, ...)
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

