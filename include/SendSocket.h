#ifndef SENDSOCKET_H
#define SENDSOCKET_H

#include <iostream>
#include <vector>
#include <winsock2.h>

#include "Socket.h"

constexpr int SEND_BUF_SIZE = 1200;

class SendSocket : public Socket {
public:
    SendSocket(DWORD flag);
    virtual void Send() = 0;
};

#endif