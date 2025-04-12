#include <iostream>
#include <chrono>

#include <ws2tcpip.h>
#include <winsock2.h>

#include "common.h"

Socket::Socket()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock.\n";
    }
}

Socket::~Socket()
{
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
    }
    WSACleanup();
}

void Socket::InitSockAddr(const char* ipAddr, int port)
{
    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_port = htons(port);
    inet_pton(AF_INET, ipAddr, &m_sockAddr.sin_addr);
}

void Socket::InitSockAddr(u_long ipAddr, int port)
{
    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_port = htons(port);
    m_sockAddr.sin_addr.s_addr = ipAddr;
}

void Socket::CreateSocket(DWORD flag)
{
    m_socket = WSASocketW(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, flag);
    if (m_socket == INVALID_SOCKET) {
        std::cout << "WSASocketW error:" << WSAGetLastError() << "\n";
        return;
    }

    u_long blockFlag = 1;
    if (ioctlsocket(m_socket, (int)FIONBIO, &blockFlag) != 0) {
        std::cout << "ioctlsocket error:" << WSAGetLastError() << "\n";
    }

    int sendBuf = SOCKET_BUFFER_SIZE;
    if (setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char *)&sendBuf, sizeof(sendBuf)) != 0) {
        std::cout << "setsockopt error:" << WSAGetLastError() << "\n";
    }

    int recvBuf = SOCKET_BUFFER_SIZE;
    if (setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char *)&recvBuf, sizeof(recvBuf)) != 0) {
        std::cout << "setsockopt error:" << WSAGetLastError() << "\n";
    }
}

void Socket::BindSocket()
{
    int ret = bind(m_socket, (sockaddr*)&m_sockAddr, sizeof(m_sockAddr));
    if (ret== SOCKET_ERROR) {
        std::cout << "bind error:" << WSAGetLastError() << "\n";
    }
}

int64_t MicrosecondsTimestamp()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    return microseconds.count();
}
