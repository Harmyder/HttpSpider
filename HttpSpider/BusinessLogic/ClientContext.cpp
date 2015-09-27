#include "stdafx.h"

#include "ClientContext.h"

using namespace std;

namespace BusinessLogic
{
    ClientContext::ClientContext(SOCKET socket, int linkIndex) : _socket(socket), _linkIndex(linkIndex)
    {
        ZeroMemory(&_ol, sizeof(_ol));
        _wbuf.len = MAX_BUFFER_LEN;
        _wbuf.buf = _buffer;
    }

    ClientContext::~ClientContext()
    {
        ReassociateWithPort();
        CloseHandle(_ol.hEvent);
        closesocket(_socket);
    }

    void ClientContext::CreateInternalEvent()
    {
        _ol.hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    void ClientContext::DisassociateWithPort()
    {
        _ol.hEvent = (HANDLE)((DWORD_PTR)_ol.hEvent | 1);
    }

    void ClientContext::ReassociateWithPort()
    {
        _ol.hEvent = (HANDLE)((DWORD_PTR)_ol.hEvent & ~1);
    }
}