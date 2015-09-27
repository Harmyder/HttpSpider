#include "stdafx.h"

#include "SDK\Algorithms\Kmp.h"
#include "SDK\Algorithms\UnitTest_Kmp.h"

#include "Utilities\UnitTests\UnitTestHelpers.h"

using namespace std;
using Utilities::UnitTests::UnitTestHelper;

namespace SDK
{
    void UnitTest_Kmp::Run()
    {
        TestComputePartialMatches();
        TestHandleTextPortion();
    }

    void UnitTest_Kmp::TestComputePartialMatches()
    {
        TestSinglePartialMatches("ABCDABD", { -1, 0, 0, 0, 0, 1, 2 });
        TestSinglePartialMatches("PARTICIPATE IN PARACHUTE", { -1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0 });


    }

    void UnitTest_Kmp::TestSinglePartialMatches(const string& pattern, vector<int> partialMatches)
    {
        _ASSERT(pattern.size() == partialMatches.size());

        vector<int> actualPartialMatches = Kmp::ComputePartialMatches(pattern);

        UnitTestHelper::CheckVectorsForEquality(actualPartialMatches, partialMatches);
    }

    void UnitTest_Kmp::TestHandleTextPortion()
    {
        string pattern = "ABCDABD";
        auto pm = Kmp::ComputePartialMatches(pattern);

        { // One match
            Kmp kmp(pattern, pm);
            vector<int> matchesStarts;
            kmp.HandleTextPortion("ABC ABCDAB ABCDABCDABDE", matchesStarts);
            UnitTestHelper::CheckVectorsForEquality<int>(matchesStarts, { 15 });
        }

        { // Splitted match
            Kmp kmp(pattern, pm);
            vector<int> matchesStarts;
            kmp.HandleTextPortion("ABC ABCDAB ABCDABC", matchesStarts);
            kmp.HandleTextPortion("DABDE", matchesStarts);
            UnitTestHelper::CheckVectorsForEquality<int>(matchesStarts, { 15 });
        }

        { // Two matches
            Kmp kmp(pattern, pm);
            vector<int> matchesStarts;
            kmp.HandleTextPortion("ABC ABCDAB ABCDABC", matchesStarts);
            kmp.HandleTextPortion("DABDE ABCDABD", matchesStarts);
            UnitTestHelper::CheckVectorsForEquality<int>(matchesStarts, { 15, 24 });
        }
    }

}