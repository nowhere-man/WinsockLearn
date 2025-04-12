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

void SendSocket::SyncSend()
{
    while (m_sendCount < TRANSIMIT_PKT_COUNT) {
        m_startTime = MicrosecondsTimestamp();
        WSABUF *wsaBuf = CreateBuf(SEND_BUF_SIZE);
        sprintf(wsaBuf->buf, "%llu", MicrosecondsTimestamp());
        m_bytesSent = 0;
        int result = WSASendTo(m_socket, wsaBuf, 1, &m_bytesSent, 0, (sockaddr*)&m_sockAddr, sizeof(sockaddr), NULL, NULL);
        if (result == SOCKET_ERROR) {
            std::cout << "WSASendTo error:" << WSAGetLastError() << "\n";
            DestoryBuf(wsaBuf);
            continue;
        }
        DestoryBuf(wsaBuf);
        m_sendCount++;
        m_totalBytesSent += m_bytesSent;
        m_timeCost += (MicrosecondsTimestamp() - m_startTime ) / 1000.f;
    }
    printf_s("sent packt count: %d, sent packet Bytes:%lu, WSASendTo cost: %f ms.\n",
        m_sendCount, m_totalBytesSent, m_timeCost);
}

void SendSocket::AsyncSend()
{
    while (m_sendCount < TRANSIMIT_PKT_COUNT) {
        m_startTime = MicrosecondsTimestamp();
        PerIoContext* ioCtx = CreateIoContext(SEND_BUF_SIZE, OP_SEND);
        ioCtx->overlapped.hEvent = WSACreateEvent();
        sprintf(ioCtx->wsaBuf.buf, "%llu", MicrosecondsTimestamp());
        m_bytesSent = 0;
        int result = WSASendTo(m_socket, &ioCtx->wsaBuf, 1, &m_bytesSent, 0, (sockaddr*)&m_sockAddr, sizeof(sockaddr), &ioCtx->overlapped, NULL);
        if (result == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSA_IO_PENDING) {
                m_pendingCount++;
                WSAWaitForMultipleEvents(1, &ioCtx->overlapped.hEvent, TRUE, WSA_INFINITE, FALSE);
                WSAGetOverlappedResult(m_socket, &ioCtx->overlapped, &m_bytesSent, FALSE, 0);
            } else {
                std::cout << "WSASendTo error:" << err << "\n";
                DestoryIoContext(ioCtx);
                continue;
            }
        }
        DestoryIoContext(ioCtx);
        m_sendCount++;
        m_totalBytesSent += m_bytesSent;
        m_timeCost += (MicrosecondsTimestamp() - m_startTime ) / 1000.f;
    }
    printf_s("sent packt count: %d, sent packet Bytes:%lu, pending count: %d, WSASendTo cost: %f ms.\n",
        m_sendCount, m_totalBytesSent, m_pendingCount, m_timeCost);
}

void SendSocket::AsyncSend_IOCP()
{
    while (m_sendCount < TRANSIMIT_PKT_COUNT) {
        m_startTime = MicrosecondsTimestamp();
        PerIoContext* ioCtx = CreateIoContext(SEND_BUF_SIZE, OP_SEND);
        sprintf(ioCtx->wsaBuf.buf, "%llu", MicrosecondsTimestamp());
        m_bytesSent = 0;
        int result = WSASendTo(m_socket, &ioCtx->wsaBuf, 1, NULL, 0, (sockaddr*)&m_sockAddr, sizeof(sockaddr), &ioCtx->overlapped, NULL);
        if (result == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSA_IO_PENDING) {
                m_pendingCount++;
            } else {
                std::cout << "WSASendTo error:" << err << "\n";
                DestoryIoContext(ioCtx);
                continue;
            }
        }
        m_sendCount++;
        m_timeCost += (MicrosecondsTimestamp() - m_startTime ) / 1000.f;
    }
    printf_s("WSASendTo: sent packt count: %d, pending count: %d, WSASendTo cost: %f ms.\n", m_sendCount, m_pendingCount, m_timeCost);
}