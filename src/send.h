#pragma once

#ifndef SEND_H
#define SEND_H

#include <iostream>
#include <vector>
#include <winsock2.h>

#include "common.h"

constexpr int SEND_BUF_SIZE = 1200;

class SendSocket : public Socket {
public:
    SendSocket(DWORD flag);
    void SyncSend();
    void AsyncSend();
    void AsyncSend_IOCP();
private:
    int64_t m_startTime{0};
    int m_sendCount{0};
    float m_timeCost{0};
    DWORD m_bytesSent{0};
    DWORD m_totalBytesSent{0};
    int m_pendingCount{0};
};

#endif