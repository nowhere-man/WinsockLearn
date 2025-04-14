#ifndef RECVSOCKET_H
#define RECVSOCKET_H

#include <iostream>
#include <vector>
#include <winsock2.h>

#include "Socket.h"
#include "IOCP.h"

constexpr int POLL_TIMEOUT = 10;    // ms
constexpr int RECV_BUF_SIZE = 1500; // Bytes

class RecvSocket : public Socket {
public:
    RecvSocket(DWORD flag);
    bool Poll();
    virtual void Recv() = 0;
private:
    pollfd m_fds[1];
    sockaddr m_recvAddr;
    int m_addrLen{sizeof(sockaddr)};
};

class SyncRecv : public RecvSocket {
public:
    void Recv() override;
};

class AsyncRecv : public RecvSocket {
public:
    void Recv() override;
};

class AsyncRecv_IOCP : public RecvSocket, public IOCP {
public:
    void Recv() override;
    void WorkerThread() override;
};

#endif