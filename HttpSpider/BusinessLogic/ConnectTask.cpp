#include "stdafx.h"

#include "BusinessLogic\ConnectTask.h"

#include <Mswsock.h>

#include "Logging\LogManager.h"
#include "BusinessLogic\ClientContext.h"
#include "Utilities\Http\HttpHelper.h"
#include "Utilities\Http\UrlParser.h"
#include "Utilities\GetErrorStrings.h"
#include "SDK\IQueue.h"
#include "SDK\AvailableItems.h"
#include "SDK\VectorImEl.h"

using namespace std;
using namespace Utilities;
using namespace SDK;

namespace BusinessLogic
{
    ConnectTask::ConnectTask(SDK::AvailableItems &ai, SDK::IQueue<int> &linksIndices, const SDK::VectorImEl<string> &links, int linksCount, HANDLE iocp) :
        StateChanged(this), ConnectionTaskChanged(this), _ai(ai), _linksIndices(linksIndices), _links(links), _linksCount(linksCount), _iocp(iocp)
    {
        InitWSA();
        LoadConnectEx();
    }

    ConnectTask::~ConnectTask()
    {
        WSACleanup();
    }

    void ConnectTask::operator()()
    {       
        bool forcedShutdown = false;
        for (int i = 0; i < _linksCount; ++i)
        {
            int linkIndex;
            forcedShutdown = !_linksIndices.PopFront(linkIndex);
            if (forcedShutdown) break;
            const string link = _links[linkIndex];

            int errorId = -1;
            StateChanged(LinkStateEventArgs(linkIndex, LinkState::STARTED, ERROR_NO_ID, 0));

            UrlComponents components = ParseUrl(link);
            const bool notUrl = components.scheme.empty() || components.host.empty();
            if (notUrl)
                goto connection_finished;

            struct sockaddr_in address;

            SOCKET sd = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
            if (sd == INVALID_SOCKET)
            {
                LOG_ADD_FMT("%s\n\tWSASocket failed: %s. Skipped.\n", link.c_str(), GetWSALastErrorString());
                goto connection_finished;
            }

            memset(&address, 0, sizeof(address));
            address.sin_family = AF_INET;
            const u_short port = components.port.length() > 0 ? (u_short)atoi(components.port.c_str()) : 80;
            address.sin_port = htons(port);

            try
            {
                ResolveHostName(components.host.c_str(), &(address.sin_addr));
            }
            catch (const ConnectTaskError& e)
            {
                LOG_ADD_FMT("%s\n\tCan't resolve host name with error: %s. Skipped.\n", link.c_str(), e.what());
                errorId = ERROR_CONNECTING_ID; goto connection_finished;
            }
            inet_pton(PF_INET, components.host.c_str(), &(address.sin_addr));

            {
                struct sockaddr_in anyaddr;
                ZeroMemory(&anyaddr, sizeof(anyaddr));
                anyaddr.sin_family = AF_INET;
                anyaddr.sin_addr.s_addr = INADDR_ANY;
                anyaddr.sin_port = 0;
                if (::bind(sd, (SOCKADDR*)&anyaddr, sizeof(anyaddr)) != 0)
                {
                    LOG_ADD_FMT("%s\n\tbind failed: %d. Skipped\n", link.c_str(), GetWSALastErrorString());
                    errorId = ERROR_CONNECTING_ID; goto connection_finished;
                }
            }

            DWORD bytesSend = static_cast<DWORD>(-1);
            ClientContext *pClientContext = new ClientContext(sd, linkIndex);
            if (_iocp != CreateIoCompletionPort((HANDLE)sd, _iocp, (ULONG_PTR)pClientContext, 0))
            {
                LOG_ADD_FMT("%s\n\tCreateIoCompletionPort failed: %d. Skipped.\n", link.c_str(), GetWSALastErrorString());
                errorId = ERROR_CONNECTING_ID; goto connection_finished;
            }

            { // Need this braces to avoid compiler error C2362
                vector<char> httpGet(HttpHelper::GetNeededBufferLength(components));
                HttpHelper::FormGetRequest(components, httpGet.data(), httpGet.size());
                forcedShutdown = !_ai.RemoveOne();
                if (forcedShutdown)
                    break;
                if (((LPFN_CONNECTEX)_connectEx)(pClientContext->GetSocket(), (struct sockaddr*)&address, sizeof(address), httpGet.data(), httpGet.size() - 1, &bytesSend, &pClientContext->GetOverlapped()))
                {
                    LOG_ADD_FMT("ConnectEx succeeded sync unexpectfully: %s.\n ", link.c_str());
                    throw "Cunt bee!";
                }
                else
                {
                    if (WSAGetLastError() == ERROR_IO_PENDING)
                    {
                        LOG_ADD_FMT("ConnectEx goes pending (%d): %s.\n", i, link.c_str());
                    }
                    else
                    {
                        LOG_ADD_FMT("ConnectEx failed: %s. Skipped %s\n", GetWSALastErrorString(), link.c_str());
                        errorId = ERROR_CONNECTING_ID; goto connection_finished;
                    }
                }
            }

        connection_finished:
            if (errorId == ERROR_CONNECTING_ID)
                StateChanged(LinkStateEventArgs(linkIndex, LinkState::FINISHED, ERROR_CONNECTING_ID, 0));
        }

        ConnectionTaskChanged(ConnectTaskEventArgs(forcedShutdown));

        return;
    }

    void ConnectTask::InitWSA()
    {
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD(2, 2);

        err = WSAStartup(wVersionRequested, &wsaData);
        if (err != 0) 
        {
            LOG_ADD_FMT("WSAStartup failed: %s\n", GetWSALastErrorString().c_str());
            goto goto_error;
        }

        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
        {
            WSACleanup();
            LOG_ADD_LINE("Could not find a usable version of Winsock.dll.");
            goto goto_error;
        }
        else
            LOG_ADD_LINE("The Winsock 2.2 dll was found okay.");

        return;
        goto_error:
        throw ConnectTaskError("WSA initialization failed due to error in WSAStartup().");
    }

    void ConnectTask::LoadConnectEx()
    {
        SOCKET sock;
        DWORD dwBytes;
        int rc;

        sock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
        if (sock == INVALID_SOCKET)
        {
            LOG_ADD_FMT("WSASocket failed: %s\n", GetWSALastErrorString().c_str());
            goto goto_error;
        }

        {
            GUID guid = WSAID_CONNECTEX;
            rc = WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
                &guid, sizeof(guid),
                &_connectEx, sizeof(_connectEx),
                &dwBytes, NULL, NULL);
            if (rc != 0)
            {
                LOG_ADD_FMT("WSAIoctl failed: %s\n", GetWSALastErrorString().c_str());
                goto goto_error;
            }
        }

        rc = closesocket(sock);
        if (rc != 0)
        {
            LOG_ADD_FMT("closesocket failed: %s\n", GetWSALastErrorString().c_str());
            goto goto_error;
        }

        LOG_ADD_LINE("ConnectEx loaded okay.");

        return;
        goto_error:
        throw ConnectTaskError("Loading of ConnectEx failed.");
    }

    int ConnectTask::ResolveHostName(const char* hostname, struct in_addr* addr)
    {
        struct addrinfo *res;

        int result = getaddrinfo(hostname, NULL, NULL, &res);
        if (result == 0) {
            memcpy(addr, &((struct sockaddr_in *) res->ai_addr)->sin_addr,
                sizeof(struct in_addr));
            freeaddrinfo(res);
        }
        else
        {
            LOG_ADD_FMT("getaddrinfo failed: %s\n", GetWSALastErrorString().c_str());
            throw ConnectTaskError("Can't resolve host name.");
        }
        return result;
    }
}
