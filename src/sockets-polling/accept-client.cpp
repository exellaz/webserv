#include "../../include/sockets-polling.h"
#include "../include/timeout.h"


// Get sockaddr, IPv4 or IPv6:
static void *getInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Add a new file descriptor to the set
static void addToPfds(std::vector<struct pollfd>& pfds, int newFd)
{
    struct pollfd pfd;

    pfd.fd = newFd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    pfds.push_back(pfd);
}

void acceptClient(std::vector<struct pollfd>& pfds, std::vector<Client>& clients, int listener)
{
    std::cout << "POLLIN: socket " << listener << '\n';

    // If listener is ready to read, handle new client
    struct sockaddr_storage remoteAddr; // Client address
    socklen_t addrLen = sizeof remoteAddr;
    int newFd = accept(listener, (struct sockaddr *)&remoteAddr, &addrLen);
    if (newFd == -1) {
        perror("accept");
        return ;
    }
    if (setNonBlocking(newFd) == -1) {
        perror("setNonBlocking (new_fd)");
        close(newFd);
        return ;
    }

    addToPfds(pfds, newFd);
    clients.push_back(Client(newFd, getNowInSeconds()));

    char remoteIp[INET6_ADDRSTRLEN];
    printf("server: new client from %s on "
        "socket %d\n", inet_ntop(remoteAddr.ss_family,
            getInAddr((struct sockaddr*)&remoteAddr),
            remoteIp, INET6_ADDRSTRLEN),
        newFd);
}