#include "stdafx.h"

#include "BusinessLogic\UnitTest_ReceiveTask.h"
#include "BusinessLogic\ReceiveTask.h"
#include "SDK\IQueue.h"
#include "SDK\AvailableItems.h"
#include "SDK\VectorImEl.h"

using namespace std;
using namespace BusinessLogic;
using namespace SDK;

namespace BusinessLogic
{
    template <class T>
    class OneThreadedQueue : public IQueue<T>
    {
    public:
        int Size() override
        {
            return _queue.size();
        }

        bool PopFront(T& t) override
        {
            t = move(_queue.front());
            _queue.pop();
            return true;
        }

        void Push(T value) override
        {
            _queue.push(value);
        }

        void Shutdown() override
        {
            throw;
        }

    private:
        queue<T> _queue;
    };

    void UnitTest_ReceiveTask::Run()
    {
        TestHandleLinks();
    }

    void UnitTest_ReceiveTask::TestHandleLinks()
    {
        TestNotEmptyAlready();
        TestSplittedScheme();
    }

    void UnitTest_ReceiveTask::TestNotEmptyAlready()
    {
        string buffer = "/file.html http://developex.com.ua/ lkjs  oier http://aaa.bbb?param http://wer";

        OneThreadedQueue<int> otq;
        AvailableItems ai;
        VectorImEl<string> links;

        set<string> uniquity;
        ReceiveTask rt(ai, uniquity, otq, links, NULL, INVALID_HANDLE_VALUE);
        string already = "http://host";
        rt.HandleLinks(already, buffer.c_str(), "");

        CHECK_EQUAL(already.c_str(), "http://wer");
        CHECK_EQUAL(otq.Size(), 3);
    }

    void UnitTest_ReceiveTask::TestSplittedScheme()
    {
        string dummyPrefix = "dummyPrefix"; // Buffer can't be too small.
        string fullScheme = "http://";
        string domain = "www.domain.com";
        string fullUrl = fullScheme + domain;

        for (unsigned i = 1; i < fullScheme.size() - 1; ++i)
        {
            OneThreadedQueue<int> otq;
            AvailableItems ai;
            VectorImEl<string> links;

            set<string> uniquity;
            ReceiveTask rt(ai, uniquity, otq, links, NULL, INVALID_HANDLE_VALUE);
            string already;
            string partialScheme(fullUrl.begin(), fullUrl.begin() + i);
            rt.HandleLinks(already, (dummyPrefix + partialScheme).c_str(), "");
            CHECK_EQUAL(already.c_str(), partialScheme.c_str());

            string leftover(fullUrl.begin() + i, fullUrl.end());
            rt.HandleLinks(already, leftover.c_str(), "");
            CHECK_EQUAL(already.c_str(), fullUrl.c_str());
            CHECK_EQUAL(otq.Size(), 0);

            rt.HandleLinks(already, (" " + dummyPrefix).c_str(), "");
            CHECK_EQUAL(already.c_str(), "");
            CHECK_EQUAL(otq.Size(), 1);
            int index;
            otq.PopFront(index);
            string actual = links[index];
            CHECK_EQUAL(actual.c_str(), fullUrl.c_str());
        }
    }

}