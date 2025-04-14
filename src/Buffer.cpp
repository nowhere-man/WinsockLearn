#include "Buffer.h"

WSABUF* CreateBuf(const int bufSize)
{
    WSABUF* wsaBuf = new WSABUF();
    wsaBuf[0].buf = new char[bufSize];
    wsaBuf[0].len = bufSize;
    return wsaBuf;
}

void DestoryBuf(WSABUF *wsaBuf)
{
    delete[] wsaBuf[0].buf;
    delete wsaBuf;
}

PerIoContext* CreateIoContext(const int bufSize, Operation oper)
{
    PerIoContext* ioContext = new PerIoContext();
    ZeroMemory(&ioContext->overlapped, sizeof(OVERLAPPED));
    ioContext->wsaBuf.buf = new char[bufSize];
    ioContext->wsaBuf.len = bufSize;
    ioContext->operation = oper;
    return ioContext;
}

void DestoryIoContext(PerIoContext* ioContext)
{
    if (ioContext) {
        delete[] ioContext->wsaBuf.buf;
        delete ioContext;
    }
}
