#ifndef IOCP_H
#define IOCP_H

#include <winsock2.h>
#include <cstdint>
#include <thread>
#include <vector>
#include <mutex>

enum CompletionKey {
    CK_STOP,
    CK_START,
};

class IOCP {
public:
    void CreateIOCP(SOCKET socket);
    void DestoryIOCP(SOCKET socket);

    virtual void WorkerThread();
private:
    HANDLE m_iocpHandle{NULL};
    CompletionKey m_completionKey{CK_START};
    std::mutex m_iocpMutex;
    std::vector<std::thread> m_workerThreads;

    uint32_t m_totalBytesTransferred{0};
    uint32_t m_transferredCount{0};
    float m_transmitTime{0};
};

#endif