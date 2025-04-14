#ifndef BUFFER_H
#define BUFFER_H

#include <winsock2.h>
#include <cstdint>
#include <vector>

enum Operation {
    OP_NONE,
    OP_SEND,
    OP_RECV,
};

struct PerIoContext {
    OVERLAPPED overlapped;
    WSABUF wsaBuf;
    Operation operation;
};

WSABUF* CreateBuf(const int bufSize);
void DestoryBuf(WSABUF *wsaBuf);

PerIoContext* CreateIoContext(const int bufSize, Operation operation);
void DestoryIoContext(PerIoContext* ioContext);

#endif