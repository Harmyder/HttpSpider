#include "stdafx.h"

#include "Utilities\Http\HttpResponseParser.h"

using namespace std;

namespace Utilities
{
    HttpResponseComponents::HttpResponseComponents()
    {
        status = -1;
    }

    int HttpResponseParser::ParseFromBuffer(const char* buffer)
    {
        if (_sealed)
            throw HttpResponseParserError("HttpResponseParser object is sealed.");

        istringstream stream(buffer);
        string line;
        bool not_done = true;
        while (not_done)
        {
            if (getline(stream, line))
            {
                if (line == "Content-Length: 39545\r")
                {
                    int i = 0;
                    i++;
                }
                AddLine(line);
                not_done = !IsLineEmpty(line);
            }
            else
            {
                throw "Ups!";
            }
        }
        return static_cast<int>(stream.tellg());
    }

    void HttpResponseParser::AddLine(const string& line)
    {
        if (_sealed)
            throw HttpResponseParserError("HttpResponseParser object is sealed.");

        if (IsLineEmpty(line))
        {
            Seal();
        }
        else if (_components.status == -1)
        {
            const int firstSpace = line.find_first_of(' ');
            const int secondSpace = line.find_first_of(' ', firstSpace + 1);

            _components.status = atoi(line.substr(firstSpace, secondSpace).c_str());
        }

    }

    void HttpResponseParser::Seal()
    {
        _sealed = true;
    }

    bool HttpResponseParser::IsLineEmpty(const std::string& line)
    {
        return line.empty() || line[0] == '\r';
    }
}