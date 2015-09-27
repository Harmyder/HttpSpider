#include "stdafx.h"

#include "HttpHelper.h"

#include "External\Mozilla\url_parse.h"
#include "Utilities\Http\UrlParser.h"

using namespace std;
using namespace External_Mozilla;

// Useful links
// http://requestmaker.com/
// http://portquiz.net:8080/
// https://www.hurl.it/

namespace Utilities
{
    int HttpHelper::GetNeededBufferLength(const UrlComponents& components)
    {
        return
            (string("GET ") + components.path + string(" HTTP/1.1\r\n"
            "Host: ") + components.host + string(":") + components.port + string("\r\n"
            "Accept: */*\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 0\r\n"
            "\r\n")).size() + 1;
    }

    void HttpHelper::FormGetRequest(const UrlComponents& components, char* buffer, int bufferLength)
    {
        if (bufferLength < GetNeededBufferLength(components))
            throw HttpHelperError("FormGetRequest failed due to small buffer.");

        strcpy_s(buffer, bufferLength,
            (string("GET ") + components.path + string(" HTTP/1.1\r\n"
            "Host: ") + components.host + string(":") + components.port + string("\r\n"
            "Accept: */*\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 0\r\n"
            "\r\n")).c_str());
    }

}
