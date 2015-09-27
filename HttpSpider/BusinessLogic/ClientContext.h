#pragma once

namespace BusinessLogic
{
#define MAX_BUFFER_LEN 1024

    class ClientContext final
    {
        OVERLAPPED  _ol;
        WSABUF      _wbuf;

        SOCKET      _socket;
        int         _linkIndex;

        char        _buffer[MAX_BUFFER_LEN];

    public:
        ClientContext(SOCKET socket, int linkIndex);
        ~ClientContext();

        void CreateInternalEvent();
        void DisassociateWithPort();
        void ReassociateWithPort();

        virtual SOCKET GetSocket() { return _socket; }
        OVERLAPPED& GetOverlapped() { return _ol; }
        WSABUF& GetWSABuffer() { return _wbuf; }
        int GetLinkIndex() const { return _linkIndex; }
    };
}
