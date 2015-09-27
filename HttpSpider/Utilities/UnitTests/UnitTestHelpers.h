#pragma once

namespace Utilities
{
    namespace UnitTests
    {
        class UnitTestHelper
        {
        public:
            template <typename T>
            static void CheckVectorsForEquality(const std::vector<T>& actual, const std::vector<T>& expected)
            {
                CHECK_EQUAL(actual.size(), expected.size());
                if (actual.size() == expected.size())
                {
                    for (size_t i = 0; i < actual.size(); i++)
                    {
                        CHECK_EQUAL(actual[i], expected[i]);
                    }
                }
            }
        };
    }
}