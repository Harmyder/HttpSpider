// HttpSpider.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <process.h>

#include "Logging\LogManager.h"
#include "Logging\OutputWindowLogger.h"
#include "Logging\DummyLogger.h"
#include "Utilities\UnitTests\UnitTestsRegistrator.h"
#include "MVVM\View\MainView.h"

using namespace std;
using namespace Logging;

void RunUnitTests()
{
    ILogger *previousLogger = LogManager::GetInstance().GetLogger();

    DummyLogger logger;
    LogManager::GetInstance().SetLogger(&logger);

    Utilities::UnitTests::UnitTestsRegistrator utr;
    utr.Register();

    for (auto &test : utr.Tests)
    {
        (*test).Run();
    }

    utr.Unregister();

    LogManager::GetInstance().SetLogger(previousLogger);
}

int _tmain(int, _TCHAR*)
{
    RunUnitTests();

    OutputWindowLogger logger;
//    DummyLogger logger;
    LogManager::GetInstance().SetLogger(&logger);

    MVVM::MainView::GetInstance().StartDialog();



	return 0;
}

