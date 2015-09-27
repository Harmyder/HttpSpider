#pragma once

#include "BusinessLogic\LinkStateEventArgs.h"

namespace SDK
{
    template <class T>
    struct IQueue;
    template <class T>
    class VectorImEl;
    class AvailableItems;
}

namespace BusinessLogic
{
    class ConnectTask;
    struct ConnectTaskEventArgs;
    class ReceiveTask;

    class Spider final
    {
    public:
        Spider();
        ~Spider();

        void SetHWND(HWND hwnd) { _hWnd = hwnd; }

        class SpiderError : public std::runtime_error {
        public:
            explicit SpiderError(const std::string& message) : runtime_error(message) {}
        };

        void Start(const std::string& startUrl, int urlsCount, int threadsCount, const std::string& text);
        void Stop();

        const char* GetLink(int i);

    private:
        void Shutdown(SDK::IQueue<int>* queue, HANDLE iocp, int threadsCount);

        void LinkStateChangeHandler(void*, const LinkStateEventArgs args);
        void ConnectTaskHandler(void*, const ConnectTaskEventArgs args);

    private:
        std::set<std::string> _uniquity;
        std::unique_ptr<SDK::IQueue<int>> _owq;
        std::unique_ptr<SDK::VectorImEl<std::string>> _links;
        std::unique_ptr<SDK::AvailableItems> _ai;
        std::vector<int> _kmpPartialMatches;
        HANDLE _iocp;
        int _threadsCount;

        std::unique_ptr<ConnectTask> _connectTask;
        std::unique_ptr<std::thread> _connectThread;
        std::vector<std::unique_ptr<ReceiveTask>> _receiveTasks;
        std::vector<std::thread> _receiveThreads;

        HWND _hWnd;
    };
}