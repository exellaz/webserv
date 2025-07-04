#include "read-request.h"

int readFromSocket(Client& client, int bufferSize)
{
    // char buf[bufferSize + 1];
    char* buf = new char[bufferSize + 1];

    ssize_t n = recv(client.fd, buf, bufferSize, 0);
    // std::cout << "n: " << n << '\n';

    if (n == 0) {
        std::cout << "RECV_CLOSED\n";
        delete[] buf;

        return RECV_CLOSED;
    }
    else if (n == -1) {
        std::cout << "RECV_AGAIN: No data available yet, will try again next iteration.\n";
        delete[] buf;
        return RECV_AGAIN;
    }
    buf[n] = '\0';
    client.appendToBuffer(buf, n);
    delete[] buf;

    return n;
}