#include <iostream>
#include <chrono>

#include <ws2tcpip.h>
#include <winsock2.h>

#include "common.h"
#include "recv.h"

RecvSocket::RecvSocket(DWORD flag)
{
    InitSockAddr(INADDR_ANY, UDP_PORT);
    CreateSocket(flag);
    BindSocket();
}

bool RecvSocket::Poll()
{
    m_fds[0].fd = m_socket;
    m_fds[0].events = POLLIN;
    m_fds[0].revents = 0;

    int ret = WSAPoll(m_fds, 1, POLL_TIMEOUT);
    if (ret < 0) {
        std::cout << "WSAPoll error:" << WSAGetLastError() << "\n";
    }

    return ret > 0;
}

void RecvSocket::CreateBuf(int wsaBufCount)
{
    m_wsaBufs.resize(wsaBufCount);
    for (int i = 0; i < wsaBufCount; ++i) {
        m_wsaBufs[i].buf = new char[RECV_BUF_SIZE];
        m_wsaBufs[i].len = RECV_BUF_SIZE;
    }
}

void RecvSocket::DestoryBuf(LPWSABUF wsaBuf, int wsaBufCount)
{
    for (int i = 0; i < wsaBufCount; ++i) {
        delete[] wsaBuf[i].buf;
    }
}

void RecvSocket::SyncRecv()
{
    int64_t startTime;
    int recvCount = 0;
    float timeCost = 0;
    DWORD byteRecv = 0;
    DWORD flags = 0;
    sockaddr recvAddr;
    int addrLen = sizeof(recvAddr);
    while (true) {
        if (!Poll()) {
            continue;
        }
        startTime = MicrosecondsTimestamp();

        //CreateBuf(WSABUF_COUNT);
        char buf[1500] = {};
        WSABUF wsaBuf;
        wsaBuf.buf = buf;
        wsaBuf.len = 1500;
        int result = WSARecvFrom(m_socket, &wsaBuf, 1, &byteRecv, &flags, &recvAddr, &addrLen, NULL, NULL);
        if (result == SOCKET_ERROR) {
            int errNo = WSAGetLastError();
            std::cout << "WSARecvFrom error:" << errNo << "\n";
            continue;
        }
        //DestoryBuf(m_wsaBufs.data(), m_wsaBufs.size());

        recvCount++;
        timeCost += (MicrosecondsTimestamp() - startTime ) / 1000.f;
        std::cout << "received packet: " << recvCount << ", time cost: " << timeCost << " ms.\n";
    }
}

void RecvSocket::AsyncRecv()
{

}