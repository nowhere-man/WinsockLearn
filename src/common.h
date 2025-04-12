#pragma once

#ifndef COMMON_H
#define COMMON_H

#include <winsock2.h>
#include <cstdint>

constexpr int UDP_PORT = 9090;
constexpr int WSABUF_COUNT = 1;
constexpr int SOCKET_BUFFER_SIZE = 1 * 1024 * 1024;

class Socket {
public:
    Socket();
    ~Socket();

    void InitSockAddr(const char* ipAddr, int port);
    void InitSockAddr(u_long ipAddr, int port);
    void CreateSocket(DWORD flag);
    void BindSocket();

protected:
    SOCKET m_socket;
    sockaddr_in m_sockAddr;
};

int64_t MicrosecondsTimestamp();

#endif