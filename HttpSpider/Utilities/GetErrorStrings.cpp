#include "stdafx.h"

#include "Utilities\GetErrorStrings.h"

using namespace std;

namespace
{
    string GetErrorString(int errCode)
    {
        LPSTR errString;

        int size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM,
            0,
            errCode,
            0,
            (LPSTR)&errString,
            0,
            0);

        if (size != 0)
        {
            std::string result(errString, errString + size);

            LocalFree(errString);

            return result;
        }

        return string();
    }
}

namespace Utilities
{
    string GetLastErrorString(int errCode)
    {
        return GetErrorString(errCode);
    }

    string GetLastErrorString()
    {
        int errCode = WSAGetLastError();
        return GetLastErrorString(errCode);
    }

    string GetWSALastErrorString(int errCode)
    {
        return GetErrorString(errCode);
    }

    string GetWSALastErrorString()
    {
        int errCode = WSAGetLastError();
        return GetWSALastErrorString(errCode);
    }
}