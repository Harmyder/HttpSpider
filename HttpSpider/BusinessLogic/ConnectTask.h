#pragma once

#include "BusinessLogic\LinkStateEventArgs.h"

namespace SDK
{
    template <class T>
    class VectorImEl;
    template <class T>
    struct IQueue;
    class AvailableItems;
}

namespace BusinessLogic
{
    struct ConnectTaskEventArgs final : MVVM::EventArgs
    {
        ConnectTaskEventArgs(bool forcedShutdown) :
            ForcedShutdown(forcedShutdown)
        {}

        bool ForcedShutdown;
    };

    DEFINE_EVENT(ConnectTaskEvent);

    class ConnectTask final
    {
        ConnectTask(const ConnectTask&) = delete;
        ConnectTask& operator=(const ConnectTask&) = delete;

    public:
        class ConnectTaskError : public std::runtime_error {
        public:
            explicit ConnectTaskError(const std::string& message) : runtime_error(message) {}
        };

    public:
        ConnectTask(SDK::AvailableItems &ai, SDK::IQueue<int> &linksIndices, const SDK::VectorImEl<std::string> &_links, int linksCount, HANDLE iocp);
        ~ConnectTask();

        void operator()();

        LinkStateEvent<ConnectTask> StateChanged;
        ConnectTaskEvent<ConnectTask> ConnectionTaskChanged;

    private:
        void InitWSA();
        void LoadConnectEx();
        int ResolveHostName(const char* hostname, struct in_addr* addr);

    private:
        SDK::AvailableItems &_ai;
        SDK::IQueue<int> &_linksIndices;
        const SDK::VectorImEl<std::string> &_links;
        void* _connectEx;
        const int _linksCount;
        const HANDLE _iocp;
    };
}