#pragma once

#ifndef SEND_H
#define SEND_H

#include <iostream>
#include <vector>
#include <winsock2.h>

#include "common.h"

constexpr int SEND_PKT_COUNT_TOTAL = 500;
constexpr int SEND_BUF_SIZE = 1000;

class SendSocket : public Socket {
public:
    SendSocket(DWORD flag);
    void SyncSend();
    void AsyncSend();
    void CreateBuf(int wsaBufCount);
    void DestoryBuf(LPWSABUF wsaBuf, int wsaBufCount);
private:
    std::vector<WSABUF> m_wsaBufs;
};

#endif