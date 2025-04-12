#pragma once

#ifndef COMMON_H
#define COMMON_H

#include <winsock2.h>
#include <cstdint>
#include <thread>
#include <vector>
#include <mutex>

constexpr int UDP_PORT = 35000;
constexpr int WSABUF_COUNT = 1;
constexpr int TRANSIMIT_PKT_COUNT = 1000;
constexpr int SOCKET_BUFFER_SIZE = 3 * 1024 * 1024;

enum Operation {
    OP_NONE,
    OP_SEND,
    OP_RECV,
};

enum CompletionKey {
    CK_STOP,
    CK_START,
};

struct PerIoContext {
    OVERLAPPED overlapped;
    WSABUF wsaBuf;
    Operation operation;
};

class Socket {
public:
    Socket();
    ~Socket();

    void InitSockAddr(const char* ipAddr, int port);
    void InitSockAddr(u_long ipAddr, int port);
    void CreateSocket(DWORD flag);
    void BindSocket();

    void CreateIOCP();
    void DestoryIOCP();
    void WorkerThread();

    WSABUF* CreateBuf(const int bufSize);
    void DestoryBuf(WSABUF *wsaBuf);
    PerIoContext* CreateIoContext(const int bufSize, Operation oper);
    void DestoryIoContext(PerIoContext* ioContext);
protected:
    SOCKET m_socket;
    sockaddr_in m_sockAddr;

    HANDLE m_iocpHandle{NULL};
    CompletionKey m_completionKey{CK_START};
    std::mutex m_iocpMutex;
    std::vector<std::thread> m_workerThreads;

    DWORD m_totalBytesTransferred{0};
    int m_transferredCount{0};
    float m_transmitTime{0};
};

int64_t MicrosecondsTimestamp();

#endif