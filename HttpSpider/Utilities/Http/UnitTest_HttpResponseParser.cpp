#include "stdafx.h"

#include "Utilities\Http\UnitTest_HttpResponseParser.h"
#include "Utilities\Http\HttpResponseParser.h"

using namespace std;

namespace Utilities
{
    void UnitTest_HttpResponseParser::Run()
    {
        char *response = R"(HTTP/1.1 200 OK
Server: nginx/1.6.2
Date: Sat, 19 Sep 2015 08:35:56 GMT
Content-Type: text/html; charset=utf-8
Transfer-Encoding: chunked
Connection: keep-alive
Vary: Accept-Encoding
Set-Cookie: PHPSESSID=aiqs6um2tk2hv6gpkoi4k8c3h7; path=/
Expires: Thu, 21 Jul 1977 07:30:00 GMT
Last-Modified: Sat, 19 Sep 2015 08:35:56 GMT
Cache-Control: post-check=0, pre-check=0
Pragma : no - cache

Message-Body)";

        HttpResponseParser rp;
        int pos = rp.ParseFromBuffer(response);
        CHECK_EQUAL(response + pos, "Message-Body");
        CHECK_EQUAL(rp.Components().status, 200);
    }
}