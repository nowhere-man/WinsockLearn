#include "src/send.h"

int main()
{
    SendSocket sendSocket(0);
    sendSocket.SyncSend();

    return 0;
}