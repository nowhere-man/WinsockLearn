#pragma once

#ifndef RECV_H
#define RECV_H

#include <iostream>
#include <vector>
#include <winsock2.h>

#include "common.h"

constexpr int POLL_TIMEOUT = 10;

constexpr int RECV_BUF_SIZE = 1500;

class RecvSocket : public Socket {
public:
    RecvSocket(DWORD flag);
    bool Poll();
    void SyncRecv();
    void AsyncRecv();
    void AsyncRecv_IOCP();
private:
    pollfd m_fds[1];
    int64_t m_startTime;
    int64_t m_endTime;
    int m_recvCount{0};
    int m_pendingCount{0};
    float m_timeCost{0};
    float m_transmitTime{0};
    DWORD m_bytesRecv{0};
    DWORD m_totalBytesRecv{0};
    DWORD m_flags{0};
    sockaddr m_recvAddr;
    int m_addrLen{sizeof(sockaddr)};
};

#endif