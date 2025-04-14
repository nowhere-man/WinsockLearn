#ifndef COMMON_H
#define COMMON_H

#include <winsock2.h>

constexpr int UDP_PORT = 35000;
constexpr int WSABUF_COUNT = 1;
constexpr int TRANSIMIT_PKT_COUNT = 1000;
constexpr int SOCKET_BUFFER_SIZE = 3 * 1024 * 1024;

class Socket {
public:
    void InitSockAddr(const char* ipAddr, int port);
    void InitSockAddr(u_long ipAddr, int port);
    void CreateSocket(DWORD flag);
    void DestorySocket();
    void SetNonBlockSocket();
    void BindSocket();
protected:
    SOCKET m_socket;
    sockaddr_in m_sockAddr;
};

#endif