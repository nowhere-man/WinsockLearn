#include "src/SendSocket.h"

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
            SendSocket sendSocket(0);
            sendSocket.SyncSend();
            break;
        }
        case 2: {
            SendSocket sendSocket(WSA_FLAG_OVERLAPPED);
            sendSocket.AsyncSend();
            break;
        }
        case 3: {
            SendSocket sendSocket(WSA_FLAG_OVERLAPPED);
            sendSocket.CreateIOCP();
            sendSocket.AsyncSend_IOCP();
            sendSocket.DestoryIOCP();
            break;
        }
        default:
            break;
    }
    return 0;
}
