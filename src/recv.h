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
    void CreateBuf(int wsaBufCount);
    void DestoryBuf(LPWSABUF wsaBuf, int wsaBufCount);
private:
    pollfd m_fds[1];
    std::vector<WSABUF> m_wsaBufs;
};

#endif