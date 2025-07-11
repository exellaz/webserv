#include <fcntl.h>
#include "handle-sockets.h"

// Helper: set a file descriptor to non-blocking mode
int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    // set file status with existing flags + Non-blocking flag
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/**
 * @note 1. getSockName() retrieves the local address of the socket
 * @note 2. ntohs() converts the port number from network byte order to host byte ( mean from big-endian to 16-bit number)
 * @note 3. sockaddr is used to store the address of the socket
 * @note 4. sockaddr_in is used to store the address of the socket in IPv4 format
*/
std::pair<std::string, std::string> getIpAndPortFromSocketFd(int fd)
{
    struct sockaddr_storage remoteAddr;
    socklen_t addrLen = sizeof(remoteAddr);
    getsockname(fd, (struct sockaddr *)&remoteAddr, &addrLen);

    int port = ntohs(((struct sockaddr_in*)&remoteAddr)->sin_port);
    std::stringstream portStream;
    portStream << port;

    int ip = ntohl(((struct sockaddr_in*)&remoteAddr)->sin_addr.s_addr);
    std::ostringstream ipStream;
    ipStream << ((ip >> 24) & 0xFF) << "."
                << ((ip >> 16) & 0xFF) << "."
                << ((ip >> 8)  & 0xFF) << "."
                << (ip & 0xFF);

    return std::make_pair(ipStream.str(), portStream.str());
}

void closeAllSockets(std::vector<struct pollfd>& pfds)
{
    std::cout << GREY << "\nClosing all sockets.\n" << RESET;
    std::vector<struct pollfd>::const_iterator it = pfds.begin();
    for (; it != pfds.end(); ++it) {
        close(it->fd);
    }
}
