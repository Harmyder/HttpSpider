#include "stdafx.h"

#include "BusinessLogic\ReceiveTask.h"

#include "BusinessLogic\ClientContext.h"
#include "Logging\LogManager.h"
#include "Utilities\GetErrorStrings.h"
#include "Utilities\Http\HttpResponseParser.h"
#include "SDK\IQueue.h"
#include "SDK\AvailableItems.h"
#include "SDK\VectorImEl.h"
#include "SDK\Algorithms\Kmp.h"
#include "BusinessLogic\LinkConstants.h"

#include <regex>
using namespace std;
using namespace Utilities;

namespace
{
    const string SCHEME_PREFIX = "http://";
}

namespace BusinessLogic
{
    ReceiveTask::ReceiveTask(SDK::AvailableItems &ai, std::set<std::string> &uniquity, SDK::IQueue<int> &linksIndices, SDK::VectorImEl<std::string> &links, const SDK::Kmp* kmp, HANDLE iocp) :
        StateChanged(this), _ai(ai), _linksIndices(linksIndices), _links(links), _uniquity(uniquity), _iocp(iocp)
    {
        if (kmp)
        {
            _kmp = unique_ptr<SDK::Kmp>(new SDK::Kmp(*kmp));
        }
    }

    ReceiveTask::~ReceiveTask() {}

    void ReceiveTask::operator()()
    {
        size_t threadId = this_thread::get_id().hash();
        for (;;)
        {
            DWORD bytesTransferred;
            OVERLAPPED *dummyOverlapped;
            ClientContext *cc;
            _ai.AddOne();
            if (FALSE == GetQueuedCompletionStatus(_iocp, &bytesTransferred, (PULONG_PTR)&cc, &dummyOverlapped, INFINITE))
            {
                LOG_ADD_FMT("%Iu: GetQueuedCompletionStatus failed: %s\n", threadId, GetLastErrorString().c_str());
            }

            // TODO: check if not all bytes transfered, isn't it always the case?

            if (NULL == cc)
            {
                break;
            }

            int found = 0;
            StateChanged(LinkStateEventArgs(cc->GetLinkIndex(), LinkState::DOWNLOADING, ERROR_NO_ID, -1));

            const char *parentLink = _links[cc->GetLinkIndex()].c_str();
            LOG_ADD_FMT("%Iu: Start handling %s\n", threadId, parentLink);
            cc->CreateInternalEvent();
            cc->DisassociateWithPort();

            const int BUF_SIZE = 1024 * 4;
            WSABUF wsaBufRecv{ BUF_SIZE, new char[BUF_SIZE] };
            DWORD bytesRead = (DWORD)-1;
            string lastPrefix;

            int errorId = ERROR_SUCCESS_ID;
            HttpResponseParser rp;
            bool not_done = true;
            while (not_done)
            {
                int wsResult;
                DWORD wsFlags = 0;
                
                string before;
                if (bytesRead != -1)
                {
                    before = string((char*)wsaBufRecv.buf);
                }

                wsResult = WSARecv(cc->GetSocket(), &wsaBufRecv, 1, &bytesRead, &wsFlags, &cc->GetOverlapped(), NULL);
                if (wsResult != 0)
                {
                    if ((wsResult == SOCKET_ERROR) && (WSA_IO_PENDING != WSAGetLastError()))
                    {
                        LOG_ADD_FMT("%Iu: WSARecv failed with error: %s. For %s.\n", threadId, GetWSALastErrorString().c_str(), parentLink);
                        errorId = ERROR_RECEIVING_ID; goto finish_page;
                    }

                    DWORD waitResult = WSAWaitForMultipleEvents(1, &cc->GetOverlapped().hEvent, FALSE, 25000, FALSE);
                    if (WAIT_FAILED == waitResult)
                    {
                        LOG_ADD_FMT("%Iu: Wait failed with error: %s. For %s.\n", threadId, GetWSALastErrorString().c_str(), parentLink);
                        errorId = ERROR_RECEIVING_ID; goto finish_page;
                    }
                    else if (WSA_WAIT_TIMEOUT == waitResult)
                    {
                        LOG_ADD_FMT("%Iu: Wait timeout: %s. For %s.\n", threadId, GetWSALastErrorString().c_str(), parentLink);
                        errorId = ERROR_RECEIVING_ID; goto finish_page;
                    }

                    if (FALSE == WSAGetOverlappedResult(cc->GetSocket(), &cc->GetOverlapped(), &bytesRead, FALSE, &wsFlags))
                    {
                        LOG_ADD_FMT("%Iu: WSAGetOverlappedResult failed with error: %s. For %s.\n", threadId, GetWSALastErrorString().c_str(), parentLink);
                        errorId = ERROR_RECEIVING_ID; goto finish_page;
                    }
                }
                else
                {
                }

                wsaBufRecv.buf[bytesRead] = 0;
                int pos = 0;
                if (!rp.Sealed())
                    pos = rp.ParseFromBuffer(wsaBufRecv.buf);

                int status = rp.Components().status;
                if (200 <= status && status < 300)
                {
                    HandleLinks(lastPrefix, wsaBufRecv.buf + pos, _links[cc->GetLinkIndex()]);
                    vector<int> matchesStarts;
                    _kmp->HandleTextPortion(string(wsaBufRecv.buf + pos, wsaBufRecv.buf + bytesRead), matchesStarts);
                    found += matchesStarts.size();
                }
                else if (300 <= status && status < 400)
                {
                    // TODO: handle redirect
                    break;
                }

                if (bytesRead != BUF_SIZE)
                    goto finish_page;
            }

            finish_page:
            StateChanged(LinkStateEventArgs(cc->GetLinkIndex(), LinkState::FINISHED, errorId, found));
            delete cc;
        }
    }

    void ReceiveTask::HandleLinks(string& already, const char* buffer, const string parentLink)
    {
        string bufstr(buffer);

        int pos = 0;

        const bool possibleSplittedScheme = !already.empty() && already.size() < SCHEME_PREFIX.size();
        if (possibleSplittedScheme)
        {
            pos = CompleteOrDiscardLeftover(already, bufstr);
        }

        while (pos != string::npos)
        {
            if (already.empty())
            {
                pos = bufstr.find(SCHEME_PREFIX, pos);
            }

            if (pos != string::npos)
            {
                TryToCompleteLink(already, pos, bufstr, parentLink);
            }
        }

        if (already.empty()) FoundSplittedScheme(already, bufstr);
    }

    void ReceiveTask::FoundSplittedScheme(std::string& already, const string& bufstr)
    {
        for (unsigned i = 1; i < min(SCHEME_PREFIX.size(), bufstr.size()); ++i)
        {
            const string suffix = bufstr.substr(bufstr.size() - i);
            if (suffix == SCHEME_PREFIX.substr(0, i))
            {
                already = suffix;
                break;
            }
        }
    }

    int ReceiveTask::CompleteOrDiscardLeftover(std::string& already, const std::string& bufstr)
    {
        int pos = 0;
        const int neededToFull = SCHEME_PREFIX.size() - already.size();
        if (bufstr.substr(0, neededToFull) == SCHEME_PREFIX.substr(SCHEME_PREFIX.size() - neededToFull))
        {
            already = SCHEME_PREFIX;
            pos = neededToFull;
        }
        else
        {
            already = "";
        }
        return pos;
    }

    void ReceiveTask::TryToCompleteLink(string& already, int& pos, const std::string& bufstr, const std::string& parentLink)
    {
        regex reg(R"([\?# '"<>])");
        smatch match;
        bool found = regex_search(bufstr.cbegin() + pos, bufstr.cend(), match, reg);
        if (found)
        {
            const int foundAtPos = match.position();
            string link;
            if (!already.empty())
            {
                link = already + bufstr.substr(pos, foundAtPos);
                already = "";
            }
            else
            {
                link = bufstr.substr(pos, foundAtPos);
            }
            StoreLinkIfUnique(link, parentLink);
            pos += foundAtPos;
        }
        else
        {
            // If bufstr is a middle part of a url then we may need '+' 
            already += bufstr.substr(pos);
            pos = string::npos;
        }
    }

    void ReceiveTask::StoreLinkIfUnique(const string& link, const string& parentLink)
    {
        size_t threadId = this_thread::get_id().hash();
        if (_uniquity.find(link) == _uniquity.end())
        {
            LOG_ADD_FMT("%Iu: [%s] Link found: %s.\n", threadId, parentLink.c_str(), link.c_str());
            _linksIndices.Push(_links.Push(link) - 1);
            _uniquity.insert(link);
        }
        else
        {
            LOG_ADD_FMT("%Iu: [%s] Already added link found: %s.\n", threadId, parentLink.c_str(), link.c_str());
        }
    }
}
