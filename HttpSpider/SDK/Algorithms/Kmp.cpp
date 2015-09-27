#include "stdafx.h"

#include "SDK\Algorithms\Kmp.h"

using namespace std;

namespace SDK
{
    vector<int> Kmp::ComputePartialMatches(const std::string& pattern)
    {
        vector<int> partialMatches(pattern.size());

        partialMatches[0] = -1;
        partialMatches[1] = 0;

        int pos = 2;
        int cnd = 0;

        while (pos < (int)pattern.size())
        {
            if (pattern[pos - 1] == pattern[cnd])
            {
                cnd++;
                partialMatches[pos] = cnd;
                pos++;
            }
            else if (cnd > 0)
            {
                cnd = partialMatches[cnd];
            }
            else
            {
                partialMatches[pos] = 0; 
                pos++;
            }
        }

        return partialMatches;
    }

    Kmp::Kmp(const string& pattern, const vector<int>& partialMatches) :
        _pattern(pattern),
        _partialMatches(partialMatches),
        _m(0), _i(0), _textLength(0)
    {}


    void Kmp::HandleTextPortion(const std::string& text, std::vector<int>& matchesStarts)
    {
        int newTextLength = _textLength + text.size();

        while (_m + _i < newTextLength)
        {
            if (_pattern[_i] == text[_m + _i - _textLength])
            {
                if (_i == (int)_pattern.size() - 1)
                {
                    matchesStarts.push_back(_m);
                    _m = _m + _i;
                    _i = 0;
                    continue;
                }
                _i++;
            }
            else
            {
                _m = _m + _i - _partialMatches[_i];
                if (_partialMatches[_i] > -1)
                    _i = _partialMatches[_i];
                else
                    _i = 0;
            }
        }

        _textLength = newTextLength;
    }
}