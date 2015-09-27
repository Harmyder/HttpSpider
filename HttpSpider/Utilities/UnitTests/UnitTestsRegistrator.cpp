#include "stdafx.h"

#include "Utilities\UnitTests\UnitTestsRegistrator.h"

#include "Utilities\Http\UnitTest_HttpResponseParser.h"
#include "External\Mozilla\UnitTest_url_parse.h"
#include "BusinessLogic\UnitTest_ReceiveTask.h"
#include "SDK\Algorithms\UnitTest_Kmp.h"

using namespace std;

namespace Utilities
{
    namespace UnitTests
    {
        void UnitTestsRegistrator::Register()
        {
            Tests.push_back(new External_Mozilla::url::UnitTest_url_parse());
            Tests.push_back(new UnitTest_HttpResponseParser());
            Tests.push_back(new BusinessLogic::UnitTest_ReceiveTask());
            Tests.push_back(new SDK::UnitTest_Kmp());
        }

        void UnitTestsRegistrator::Unregister()
        {
            for (UnitTest* ut : Tests)
                delete ut;
        }
    }
}