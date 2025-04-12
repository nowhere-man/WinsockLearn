#include "src/recv.h"

int main()
{
    RecvSocket recvSocket(0);
    recvSocket.SyncRecv();

    return 0;
}