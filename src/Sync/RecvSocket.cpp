#include <iostream>
#include <chrono>
#include <string>

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

void RecvSocket::SyncRecv()
{
    while (true) {
        if (!Poll()) {
            continue;
        }
        m_startTime = MicrosecondsTimestamp();
        WSABUF *wsaBuf = CreateBuf(RECV_BUF_SIZE);
        m_bytesRecv = 0;
        int result = WSARecvFrom(m_socket, wsaBuf, 1, &m_bytesRecv, &m_flags, &m_recvAddr, &m_addrLen, NULL, NULL);
        if (result == SOCKET_ERROR) {
            int errNo = WSAGetLastError();
            std::cout << "WSARecvFrom error:" << errNo << "\n";
            continue;
        }
        m_endTime = MicrosecondsTimestamp();
        int64_t sendTime = std::stoll(wsaBuf->buf);
        m_transmitTime += (m_endTime - sendTime) / 1000.f;
        m_recvCount++;
        m_timeCost += (m_endTime - m_startTime ) / 1000.f;
        m_totalBytesRecv += m_bytesRecv;

        DestoryBuf(wsaBuf);
        if (m_recvCount >= TRANSIMIT_PKT_COUNT) {
            break;
        }
    }
    printf_s("received packt count: %d, received packet Bytes:%lu, WSARecvFrom cost: %f ms, total transmit time: %f ms.\n",
        m_recvCount, m_totalBytesRecv, m_timeCost, m_transmitTime);
}

void RecvSocket::AsyncRecv()
{
    while (true) {
        if (!Poll()) {
            continue;
        }
        m_startTime = MicrosecondsTimestamp();
        PerIoContext* ioCtx = CreateIoContext(RECV_BUF_SIZE, OP_RECV);
        ioCtx->overlapped.hEvent = WSACreateEvent();
        m_bytesRecv = 0;
        int result = WSARecvFrom(m_socket, &ioCtx->wsaBuf, 1, &m_bytesRecv, &m_flags, &m_recvAddr, &m_addrLen, &ioCtx->overlapped, NULL);
        if (result == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSA_IO_PENDING) {
                m_pendingCount++;
                WSAWaitForMultipleEvents(1, &ioCtx->overlapped.hEvent, TRUE, WSA_INFINITE, FALSE);
                WSAGetOverlappedResult(m_socket, &ioCtx->overlapped, &m_bytesRecv, FALSE, 0);
            } else {
                std::cout << "WSASendTo error:" << err << "\n";
                DestoryIoContext(ioCtx);
                continue;
            }
        }
        m_endTime = MicrosecondsTimestamp();
        int64_t sendTime = std::stoll(ioCtx->wsaBuf.buf);
        m_transmitTime += (m_endTime - sendTime) / 1000.f;
        m_recvCount++;
        m_timeCost += (m_endTime - m_startTime ) / 1000.f;
        m_totalBytesRecv += m_bytesRecv;

        DestoryIoContext(ioCtx);
        if (m_recvCount >= TRANSIMIT_PKT_COUNT) {
            break;
        }
    }

    printf_s("received packt count: %d, received packet Bytes:%lu, pending count: %d, WSARecvFrom cost: %f ms, total transmit time: %f ms.\n",
        m_recvCount, m_totalBytesRecv, m_pendingCount, m_timeCost, m_transmitTime);
}

void RecvSocket::AsyncRecv_IOCP()
{
    while (true) {
        if (!Poll()) {
            continue;
        }
        m_startTime = MicrosecondsTimestamp();
        PerIoContext* ioCtx = CreateIoContext(RECV_BUF_SIZE, OP_RECV);
        ioCtx->overlapped.hEvent = WSACreateEvent();
        m_bytesRecv = 0;
        int result = WSARecvFrom(m_socket, &ioCtx->wsaBuf, 1, &m_bytesRecv, &m_flags, &m_recvAddr, &m_addrLen, &ioCtx->overlapped, NULL);
        if (result == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSA_IO_PENDING) {
                m_pendingCount++;
            } else {
                std::cout << "WSARecvFrom error:" << err << "\n";
                DestoryIoContext(ioCtx);
                continue;
            }
        }
        m_recvCount++;
        m_timeCost += (MicrosecondsTimestamp() - m_startTime ) / 1000.f;

        if (m_recvCount >= TRANSIMIT_PKT_COUNT) {
            break;
        }
    }

    printf_s("WSARecvFrom: received packt count: %d, pending count: %d, WSARecvFrom cost: %f ms.\n",
        m_recvCount, m_pendingCount, m_timeCost);
}
