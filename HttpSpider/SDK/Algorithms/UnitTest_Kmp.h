#pragma once

#include "Utilities\UnitTests\UnitTest.h"

namespace SDK
{
    class UnitTest_Kmp : public Utilities::UnitTests::UnitTest
    {
        void Run() override;

    private:
        void TestComputePartialMatches();
        void TestSinglePartialMatches(const std::string& pattern, std::vector<int> partialMatches);

        void TestHandleTextPortion();
    };
}
