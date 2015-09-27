#include "stdafx.h"

#include "UnitTest_url_parse.h"
#include "url_parse.h"

using namespace std;

using namespace External_Mozilla;

// https://developer.mozilla.org/en-US/Learn/Understanding_URLs

namespace External_Mozilla
{
    namespace url
    {
        void UnitTest_url_parse::Run()
        {
            {
                string url = "http://www.somedomain.com:8080/path/page.thml?param1=value1&param2=value2#anchor";
                url::Parsed parsed;
                url::ParseStandardURL(url.c_str(), url.length(), &parsed);
                CHECK_EQUAL(url.substr(parsed.scheme.begin, parsed.scheme.len).c_str(), "http");
                CHECK_EQUAL(url.substr(parsed.port.begin, parsed.port.len).c_str(), "8080");
            }

            {
                string url = "https://www.somedomain.com";
                url::Parsed parsed;
                url::ParseStandardURL(url.c_str(), url.length(), &parsed);
                CHECK_EQUAL(url.substr(parsed.scheme.begin, parsed.scheme.len).c_str(), "https");
            }
        }
    }
}