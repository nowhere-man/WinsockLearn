#include <iostream>
#include <chrono>
#include <string>

#include <ws2tcpip.h>
#include <winsock2.h>

#include "common.h"
#include <cassert>

Socket::Socket()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock.\n";
    }
}

Socket::~Socket()
{
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
    }
    if (m_iocpHandle != NULL) {
        CloseHandle(m_iocpHandle);
    }
    WSACleanup();
}

void Socket::InitSockAddr(const char* ipAddr, int port)
{
    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_port = htons(port);
    inet_pton(AF_INET, ipAddr, &m_sockAddr.sin_addr);
}

void Socket::InitSockAddr(u_long ipAddr, int port)
{
    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_port = htons(port);
    m_sockAddr.sin_addr.s_addr = ipAddr;
}

void Socket::CreateSocket(DWORD flag)
{
    m_socket = WSASocketW(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, flag);
    if (m_socket == INVALID_SOCKET) {
        std::cout << "WSASocketW error:" << WSAGetLastError() << "\n";
        return;
    }

    u_long blockFlag = 1;
    if (ioctlsocket(m_socket, (int)FIONBIO, &blockFlag) != 0) {
        std::cout << "ioctlsocket error:" << WSAGetLastError() << "\n";
    }

    int sendBuf = SOCKET_BUFFER_SIZE;
    if (setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char *)&sendBuf, sizeof(sendBuf)) != 0) {
        std::cout << "setsockopt error:" << WSAGetLastError() << "\n";
    }

    int recvBuf = SOCKET_BUFFER_SIZE;
    if (setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char *)&recvBuf, sizeof(recvBuf)) != 0) {
        std::cout << "setsockopt error:" << WSAGetLastError() << "\n";
    }
}

void Socket::BindSocket()
{
    int ret = bind(m_socket, (sockaddr*)&m_sockAddr, sizeof(m_sockAddr));
    if (ret== SOCKET_ERROR) {
        std::cout << "bind error:" << WSAGetLastError() << "\n";
    }
}

void Socket::CreateIOCP()
{
    m_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, m_completionKey, 0);
    if (m_iocpHandle == NULL) {
        std::cout << "CreateIoCompletionPort 1 error:" << WSAGetLastError() << "\n";
    }

    if (CreateIoCompletionPort((HANDLE)m_socket, m_iocpHandle, m_completionKey, 0) == NULL) {
        std::cout << "CreateIoCompletionPort 2 error:" << WSAGetLastError() << "\n";
    }

    int threadNums = std::thread::hardware_concurrency();
    for (int i = 0; i < threadNums; ++i) {
        m_workerThreads.emplace_back(&Socket::WorkerThread, this);
    }
}

void Socket::DestoryIOCP()
{
    for (size_t i = 0; i < m_workerThreads.size(); ++i) {
        if (!PostQueuedCompletionStatus(m_iocpHandle, 0, CK_STOP, NULL)) {
            std::cout << "PostQueuedCompletionStatus error:" << WSAGetLastError() << "\n";
        }
    }
    for (auto& thread : m_workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    printf_s("IOCP: transferrd packt count: %d, transferrd packet bytes:%lu, total transmit time (for receiver): %f ms.\n",
        m_transferredCount, m_totalBytesTransferred, m_transmitTime);
}

void Socket::WorkerThread()
{
    DWORD bytesTransferred = 0;
    int64_t sendTime;
    int64_t recvTime;
    ULONG_PTR completionKey = 0;
    PerIoContext* ioCtx = nullptr;
    BOOL result;
    while (true) {
        result = GetQueuedCompletionStatus(m_iocpHandle, &bytesTransferred, &completionKey, reinterpret_cast<LPOVERLAPPED*>(&ioCtx), INFINITE);
        if (!result) {
            std::cout << "GetQueuedCompletionStatus error:" << WSAGetLastError() << "\n";
            continue;
        }
        if (completionKey == CK_STOP) {
            break;
        }
        if (!ioCtx) {
             std::cout << "IO Context is null.\n";
             break;
        }
        switch (ioCtx->operation) {
            case OP_SEND: {
                {
                    std::lock_guard<std::mutex> guard(m_iocpMutex);
                    m_totalBytesTransferred += bytesTransferred;
                    m_transferredCount++;
                }
                DestoryIoContext(ioCtx);
                break;
            }
            case OP_RECV: {
                sendTime = std::stoll(ioCtx->wsaBuf.buf);
                recvTime = MicrosecondsTimestamp();
                {
                    std::lock_guard<std::mutex> guard(m_iocpMutex);
                    m_totalBytesTransferred += bytesTransferred;
                    m_transferredCount++;
                    m_transmitTime += (recvTime - sendTime) / 1000.f;
                }
                DestoryIoContext(ioCtx);
                break;
            }
            default:
                std::cout << "Unknown operation type: " << ioCtx->operation << "\n";
                DestoryIoContext(ioCtx);
                break;
        }
    }
}

WSABUF *Socket::CreateBuf(const int bufSize)
{
    WSABUF* wsaBuf = new WSABUF();
    wsaBuf[0].buf = new char[bufSize];
    wsaBuf[0].len = bufSize;
    return wsaBuf;
}

void Socket::DestoryBuf(WSABUF *wsaBuf)
{
    delete[] wsaBuf[0].buf;
    delete wsaBuf;
}

PerIoContext* Socket::CreateIoContext(const int bufSize, Operation oper)
{
    PerIoContext* ioContext = new PerIoContext();
    ZeroMemory(&ioContext->overlapped, sizeof(OVERLAPPED));
    ioContext->wsaBuf.buf = new char[bufSize];
    ioContext->wsaBuf.len = bufSize;
    ioContext->operation = oper;
    return ioContext;
}

void Socket::DestoryIoContext(PerIoContext* ioContext)
{
    if (ioContext) {
        delete[] ioContext->wsaBuf.buf;
        delete ioContext;
    }
}

int64_t MicrosecondsTimestamp()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    return microseconds.count();
}
