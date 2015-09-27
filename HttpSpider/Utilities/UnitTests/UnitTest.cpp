#include "stdafx.h"

#include "UnitTest.h"
#include "Utilities\DebugPrint.h"
#include <stdlib.h>

namespace
{
    char* filename(char* path)
    {
        char filename[256];
        char extension[256];

        _splitpath_s(path,
            NULL, 0,
            NULL, 0,
            filename, sizeof(filename),
            extension, sizeof(extension));

        int pathlen = strlen(path);
        int filenamelen = strnlen_s(filename, sizeof(filename));
        int extensionlen = strnlen_s(extension, sizeof(extension));
        return path + pathlen - filenamelen - extensionlen;
    }

    void CheckFailed(char *file, int line)
    {
        DebugBreak();
        Utilities::DebugPrintf("%s: %d\n", filename(file), line);
    }
}

namespace Utilities
{
    namespace UnitTests
    {
        UnitTest::UnitTest()
        {
        }

        void UnitTest::CheckNotEqual(void* actual, void* expected, char *file, int line)
        {
            if (actual == expected)
            {
                CheckFailed(file, line);
                DebugPrintf("\tNot expected %p, but actual is %p.\n", expected, actual);
            }
        }

        void UnitTest::CheckEqual(const char* actual, const char* expected, char *file, int line)
        {
            if (strcmp(actual, expected))
            {
                CheckFailed(file, line);
                DebugPrintf("\tExpected %p, but actual is %p.\n", expected, actual);
            }
        }

        void UnitTest::CheckEqual(const void* actual, const void* expected, char *file, int line)
        {
            if (actual != expected) 
            {
                CheckFailed(file, line);
                DebugPrintf("\tExpected %p, but actual is %p.\n", expected, actual);
            }
        }

        void UnitTest::CheckEqual(int actual, int expected, char *file, int line)
        {
            if (actual != expected)
            {
                CheckFailed(file, line);
                DebugPrintf("\tExpected %d, but actual is %d.\n", expected, actual);
            }
        }
        void UnitTest::CheckEqual(size_t actual, size_t expected, char *file, int line)
        {
            CheckEqual((int)actual, (int)expected, file, line);
        }

        void UnitTest::CheckEqual(bool actual, bool expected, char *file, int line)
        {
            if (actual != expected)
            {
                CheckFailed(file, line);
                DebugPrintf("\tExpected %s, but actual is %s.\n", expected ? "true" : "false", actual ? "true" : "false");
            }
        }

        void UnitTest::CheckTrue(bool value, char *file, int line)
        {
            if (!value)
            {
                CheckFailed(file, line);
                DebugPrintf("\nTrue expected.\n");
            }
        }

        void UnitTest::CheckFalse(bool value, char *file, int line)
        {
            if (value)
            {
                CheckFailed(file, line);
                DebugPrintf("\nFalse expected.\n");
            }
        }
    }
}