#include "pair.h"

int SocketPair(SOCKET socks[2], bool overlapped) {
#if defined(_win_)
    struct sockaddr_in addr;
    SOCKET listener;
    int e;
    int addrlen = sizeof(addr);
    DWORD flags = (overlapped ? WSA_FLAG_OVERLAPPED : 0);

    if (socks == 0) {
        WSASetLastError(WSAEINVAL);

        return SOCKET_ERROR;
    }

    socks[0] = INVALID_SOCKET;
    socks[1] = INVALID_SOCKET;

    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)  {
        return SOCKET_ERROR;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0x7f000001);
    addr.sin_port = 0;

    e = bind(listener, (const struct sockaddr*)&addr, sizeof(addr));

    if (e == SOCKET_ERROR) {
        e = WSAGetLastError();
        closesocket(listener);
        WSASetLastError(e);

        return SOCKET_ERROR;
    }

    e = getsockname(listener, (struct sockaddr*)&addr, &addrlen);

    if (e == SOCKET_ERROR) {
        e = WSAGetLastError();
        closesocket(listener);
        WSASetLastError(e);

        return SOCKET_ERROR;
    }

    do {
        if (listen(listener, 1) == SOCKET_ERROR)
            break;

        if ((socks[0] = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, flags)) == INVALID_SOCKET)
            break;

        if (connect(socks[0], (const struct sockaddr*) &addr, sizeof(addr)) == SOCKET_ERROR)
            break;

        if ((socks[1] = accept(listener, NULL, NULL)) == INVALID_SOCKET)
            break;

        closesocket(listener);

        return 0;
    } while (0);

    e = WSAGetLastError();
    closesocket(listener);
    closesocket(socks[0]);
    closesocket(socks[1]);
    WSASetLastError(e);

    return SOCKET_ERROR;
#else
    (void)overlapped;

    return socketpair(AF_LOCAL, SOCK_STREAM, 0, socks);
#endif
}
