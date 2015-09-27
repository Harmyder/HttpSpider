#pragma once

#include "UnitTest.h"

namespace Utilities
{
    namespace UnitTests
    {
        class UnitTestsRegistrator
        {
        public:
            void Register();
            void Unregister();

            std::list<UnitTest*> Tests;
        };
    }
}
