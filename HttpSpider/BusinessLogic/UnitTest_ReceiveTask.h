#pragma once

#include "Utilities\UnitTests\UnitTest.h"

namespace BusinessLogic
{
    class UnitTest_ReceiveTask : public Utilities::UnitTests::UnitTest
    {
        virtual void Run();

    private:
        void TestHandleLinks();

        void TestNotEmptyAlready();
        void TestSplittedScheme();
    };
}
