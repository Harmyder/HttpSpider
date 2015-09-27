#pragma once

#include "BusinessLogic\LinkStateEventArgs.h"

namespace SDK
{
    template <class T>
    class VectorImEl;
    template <class T>
    struct IQueue;
    class AvailableItems;
    class Kmp;
}

namespace BusinessLogic
{
    class ReceiveTask final
    {
        ReceiveTask& operator=(const ReceiveTask&) = delete;
        ReceiveTask(const ReceiveTask& other) = delete;

    public:
        ReceiveTask(SDK::AvailableItems &ai, std::set<std::string> &uniquity, SDK::IQueue<int> &linksIndices, SDK::VectorImEl<std::string> &links, const SDK::Kmp* kmp, HANDLE iocp);
        ~ReceiveTask();

        void operator()();

        LinkStateEvent<ReceiveTask> StateChanged;

    private:
        void HandleLinks(std::string& already, const char* buffer, const std::string url);
        void FoundSplittedScheme(std::string& already, const std::string& bufstr);
        int CompleteOrDiscardLeftover(std::string& already, const std::string& bufstr);
        void TryToCompleteLink(std::string& already, int& pos, const std::string& bufstr, const std::string& url);

        void StoreLinkIfUnique(const std::string& link, const std::string& parentLink);

    private:
        SDK::AvailableItems &_ai;
        SDK::IQueue<int> &_linksIndices;
        SDK::VectorImEl<std::string> &_links;
        std::set<std::string> &_uniquity;
        std::unique_ptr<SDK::Kmp> _kmp;
        HANDLE _iocp;

        friend class UnitTest_ReceiveTask;
    };
}