#include "sockets-polling.h"

// Helper: set a file descriptor to non-blocking mode
int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    // set file status with existing flags + Non-blocking flag
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
