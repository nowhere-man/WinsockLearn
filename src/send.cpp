#include <iostream>
#include <chrono>

#include <ws2tcpip.h>
#include <winsock2.h>

#include "common.h"
#include "send.h"

const char* DST_IPADDR = "127.0.0.1";

SendSocket::SendSocket(DWORD flag)
{
    InitSockAddr(DST_IPADDR, UDP_PORT);
    CreateSocket(flag);
}

void SendSocket::CreateBuf(int wsaBufCount)
{
    m_wsaBufs.resize(wsaBufCount);
    for (int i = 0; i < wsaBufCount; ++i) {
        m_wsaBufs[i].buf = new char[SEND_BUF_SIZE];
        m_wsaBufs[i].len = SEND_BUF_SIZE;
        sprintf(m_wsaBufs[i].buf, "%llu", MicrosecondsTimestamp());
    }
}

void SendSocket::DestoryBuf(LPWSABUF wsaBuf, int wsaBufCount)
{
    for (int i = 0; i < wsaBufCount; ++i) {
        delete[] wsaBuf[i].buf;
    }
}

void SendSocket::SyncSend()
{
    int64_t startTime;
    int sendCount = 0;
    float timeCost = 0;
    DWORD bytesSent = 0;
    while (sendCount < TRANSIMIT_PKT_COUNT) {
        startTime = MicrosecondsTimestamp();
        CreateBuf(WSABUF_COUNT);
        int result = WSASendTo(m_socket, m_wsaBufs.data(), m_wsaBufs.size(), &bytesSent, 0, (sockaddr*)&m_sockAddr, sizeof(sockaddr), NULL, NULL);
        if (result == SOCKET_ERROR) {
            std::cout << "WSASendTo error:" << WSAGetLastError() << "\n";
            continue;
        }
        DestoryBuf(m_wsaBufs.data(), m_wsaBufs.size());
        sendCount++;
        timeCost += (MicrosecondsTimestamp() - startTime ) / 1000.f;
    }
    std::cout << "sent packet: " << sendCount << ", time cost: " << timeCost << " ms.\n";
}

void SendSocket::AsyncSend()
{

}
