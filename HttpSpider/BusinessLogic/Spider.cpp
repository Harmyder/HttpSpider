#include "stdafx.h"

#include "BusinessLogic\Spider.h"

#include "Logging\LogManager.h"
#include "BusinessLogic\ConnectTask.h"
#include "BusinessLogic\ReceiveTask.h"
#include "Utilities\GetErrorStrings.h"
#include "SDK\OneWriterQueue.h"
#include "SDK\VectorImEl.h"
#include "SDK\AvailableItems.h"
#include "SDK\Algorithms\Kmp.h"

using namespace std;
using namespace SDK;

namespace BusinessLogic
{
    Spider::Spider()
    {
    }

    Spider::~Spider() {}


    void Spider::Start(const string& startUrl, int urlsCount, int threadsCount, const string& text)
    {
        UNREFERENCED_PARAMETER(text);
        _owq = unique_ptr<OneWriterQueue<int>>(new OneWriterQueue<int>());
        _links = unique_ptr<VectorImEl<string>>(new VectorImEl<string>());
        _ai = unique_ptr<AvailableItems>(new AvailableItems());
        _kmpPartialMatches = Kmp::ComputePartialMatches(text);
        SDK::Kmp kmp(text, _kmpPartialMatches);

        _uniquity.insert(startUrl);
        _owq->Push(_links->Push(startUrl) - 1);
        _threadsCount = threadsCount;

        _iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        if (NULL == _iocp)
        {
            LOG_ADD_FMT("Error occurred while creating IOCP: %s\n", Utilities::GetLastErrorString());
            throw SpiderError("Can't start spider.");
        }

        _ai->Add(_threadsCount);

        _receiveTasks.resize(_threadsCount);
        try
        {
            for (int i = 0; i < _threadsCount; ++i)
            {
                _receiveTasks[i] = make_unique<ReceiveTask>(*_ai, _uniquity, *_owq, *_links, &kmp, _iocp);
                _receiveTasks[i]->StateChanged += std::bind(&Spider::LinkStateChangeHandler, this, placeholders::_1, placeholders::_2);
                _receiveThreads.emplace_back(ref(*_receiveTasks[i]));
            }
        }
        catch (const system_error& e)
        {
            LOG_ADD_FMT("async failed with error %s.\n", e.what());
            throw SpiderError("Can't start spider.");
        }

        _connectTask = make_unique<ConnectTask>(*_ai, *_owq, *_links, urlsCount, _iocp);
        _connectTask->StateChanged += std::bind(&Spider::LinkStateChangeHandler, this, placeholders::_1, placeholders::_2);
        _connectTask->ConnectionTaskChanged += std::bind(&Spider::ConnectTaskHandler, this, placeholders::_1, placeholders::_2);
        try
        {
            _connectThread = make_unique<thread>(ref(*_connectTask));
        }
        catch (const system_error& e)
        {
            LOG_ADD_FMT("Couldn't start thread with error %s.\n", e.what());
            throw SpiderError("Can't start spider.");
        }
    }

    void Spider::Stop()
    {
        Shutdown(_owq.get(), _iocp, _threadsCount);

        _owq.reset(nullptr);
        _ai.reset(nullptr);
        // We don't delete _links here to let UI be able to fetch them while processing belated messages
    }

    void Spider::Shutdown(SDK::IQueue<int>* queue, HANDLE iocp, int threadsCount)
    {
        LOG_ADD_LINE("SHUTDOWNING");
        queue->Shutdown();
        _ai->Shutdown();

        for (int i = 0; i < threadsCount; i++)
        {
            PostQueuedCompletionStatus(iocp, 0, (DWORD)NULL, NULL);
        }

        _connectThread->join();

        for (int i = 0; i < threadsCount; i++)
        {
            _receiveThreads[i].join();
        }
        _receiveThreads.clear();
        _receiveTasks.clear();

        // We must destruct connection object last to let receiving threads to finish
        _connectTask.reset(nullptr);
        _connectThread.reset(nullptr);

        _uniquity.clear();
    }

    void Spider::LinkStateChangeHandler(void*, const LinkStateEventArgs args)
    {
        int messageId = -1;
        switch (args.State)
        {
        case LinkState::STARTED:
            messageId = UWM_STARTED;
            break;
        case LinkState::DOWNLOADING:
            messageId = UWM_DOWNLOADING;
            break;
        case LinkState::FINISHED:
            messageId = UWM_FINISHED;
            break;
        default:
            _ASSERT(false);
        }
        PostMessage(_hWnd, messageId, (WPARAM)args.LinkIndex, MAKELPARAM(args.ErrorId, args.FoundCount));
    }

    void Spider::ConnectTaskHandler(void*, const ConnectTaskEventArgs args)
    {
        if (!args.ForcedShutdown)
            PostMessage(_hWnd, UWM_CONNECT_TASK_FINISHED, 0, 0);
    }

    const char* Spider::GetLink(int i)
    {
        return (*_links)[i].c_str();
    }
}