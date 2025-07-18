#include "client.h"
#include <sys/socket.h>

int Client::readFromSocket(int bufferSize)
{
    // char buf[bufferSize + 1];
    char* buf = new char[bufferSize + 1];

    ssize_t n = recv(_fd, buf, bufferSize, 0);

    if (n == 0) {
        // std::cout << "RECV_CLOSED\n";
        delete[] buf;

        return RECV_CLOSED;
    }
    else if (n == -1) {
        std::cout << infoTime() << "RECV_AGAIN: No data available yet, will try again next iteration.\n";
        delete[] buf;
        return RECV_AGAIN;
    }
    buf[n] = '\0';
    _buffer.append(buf, n);

    delete[] buf;

    return n;
}
