#include "src/RecvSocket.h"

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#endif

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Wrong Usage.\n";
        return 1;
    }

    int option = atoi(argv[1]);
    switch (option) {
        case 1: {
            RecvSocket recvSocket(0);
            recvSocket.SyncRecv();
            break;
        }
        case 2: {
            RecvSocket recvSocket(WSA_FLAG_OVERLAPPED);
            recvSocket.AsyncRecv();
            break;
        }
        case 3: {
            RecvSocket recvSocket(WSA_FLAG_OVERLAPPED);
            recvSocket.CreateIOCP();
            recvSocket.AsyncRecv_IOCP();
            recvSocket.DestoryIOCP();
            break;
        }
        default:
            break;
    }
    return 0;
}