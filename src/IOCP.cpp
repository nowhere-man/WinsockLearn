#include <iostream>

#include "Socket.h"
#include "Buffer.h"
#include "Timer.h"

#include "IOCP.h"

void IOCP::CreateIOCP(SOCKET socket)
{
    m_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, m_completionKey, 0);
    if (m_iocpHandle == NULL) {
        std::cout << "Create IoCompletionPort error:" << WSAGetLastError() << "\n";
    }

    if (CreateIoCompletionPort((HANDLE)socket, m_iocpHandle, m_completionKey, 0) == NULL) {
        std::cout << "Bind IoCompletionPort error:" << WSAGetLastError() << "\n";
    }

    int threadNums = std::thread::hardware_concurrency();
    for (int i = 0; i < threadNums; ++i) {
        m_workerThreads.emplace_back(&IOCP::WorkerThread, this);
    }
}

void IOCP::DestoryIOCP(SOCKET socket)
{
    for (size_t i = 0; i < m_workerThreads.size(); ++i) {
        if (!PostQueuedCompletionStatus(m_iocpHandle, 0, CK_STOP, NULL)) {
            std::cout << "PostQueuedCompletionStatus error:" << WSAGetLastError() << "\n";
        }
    }

    for (auto& thread : m_workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    CloseHandle(m_iocpHandle);

    printf_s("IOCP: transferrd packt count: %d, transferrd packet bytes:%lu, total transmit time (for receiver): %f ms.\n",
        m_transferredCount, m_totalBytesTransferred, m_transmitTime);
}

void IOCP::WorkerThread()
{
    DWORD bytesTransferred = 0;
    int64_t sendTime;
    int64_t recvTime;
    ULONG_PTR completionKey = 0;
    PerIoContext* ioCtx = nullptr;
    BOOL result;
    while (true) {
        result = GetQueuedCompletionStatus(m_iocpHandle, &bytesTransferred, &completionKey, reinterpret_cast<LPOVERLAPPED*>(&ioCtx), INFINITE);
        if (!result) {
            std::cout << "GetQueuedCompletionStatus error:" << WSAGetLastError() << "\n";
            continue;
        }
        if (completionKey == CK_STOP) {
            break;
        }
        if (!ioCtx) {
             std::cout << "IO Context is null.\n";
             break;
        }
        switch (ioCtx->operation) {
            case OP_SEND: {
                {
                    std::lock_guard<std::mutex> guard(m_iocpMutex);
                    m_totalBytesTransferred += bytesTransferred;
                    m_transferredCount++;
                }
                DestoryIoContext(ioCtx);
                break;
            }
            case OP_RECV: {
                sendTime = std::stoll(ioCtx->wsaBuf.buf);
                recvTime = MicrosecondsTimestamp();
                {
                    std::lock_guard<std::mutex> guard(m_iocpMutex);
                    m_totalBytesTransferred += bytesTransferred;
                    m_transferredCount++;
                    m_transmitTime += (recvTime - sendTime) / 1000.f;
                }
                DestoryIoContext(ioCtx);
                break;
            }
            default:
                std::cout << "Unknown operation type: " << ioCtx->operation << "\n";
                DestoryIoContext(ioCtx);
                break;
        }
    }
}
