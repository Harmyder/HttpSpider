// Knuth–Morris–Pratt
#pragma once

namespace SDK
{
    class Kmp
    {
        Kmp& operator=(const Kmp& other) = delete;

    public:
        Kmp(const std::string& pattern, const std::vector<int>& partialMatches);

        void HandleTextPortion(const std::string& text, std::vector<int>& matchesStarts);

        static std::vector<int> ComputePartialMatches(const std::string& pattern);

    private:
        const std::string& _pattern;
        const std::vector<int>& _partialMatches;

        int _m;
        int _i;
        int _textLength;
    };
}