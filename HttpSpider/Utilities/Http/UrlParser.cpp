#include "stdafx.h"

#include "Utilities\Http\UrlParser.h"

#include "External\Mozilla\url_parse.h"

using namespace std;
using namespace External_Mozilla;

namespace Utilities
{
    UrlComponents ParseUrl(const std::string& url)
    {
        url::Parsed parsed;
        url::ParseStandardURL(url.c_str(), url.length(), &parsed);

#define EXTRACT_COMPONENT(name) parsed.name.len == -1 ? string() : url.substr(parsed.name.begin, parsed.name.len)
        return UrlComponents{
            EXTRACT_COMPONENT(scheme),
            EXTRACT_COMPONENT(username),
            EXTRACT_COMPONENT(password),
            EXTRACT_COMPONENT(host),
            parsed.port.len == -1 ? string("80") : url.substr(parsed.port.begin, parsed.port.len),
            EXTRACT_COMPONENT(path),
            EXTRACT_COMPONENT(query),
            EXTRACT_COMPONENT(ref)
        };
#undef EXTRACT_COMPONENT
    }
}
