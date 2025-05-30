#include "sockets-polling.h"


// Get sockaddr, IPv4 or IPv6:
void *getInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



// Add a new file descriptor to the set
void addToPfds(std::vector<struct pollfd>& pfds, int newFd)
{
	struct pollfd pfd;

	pfd.fd = newFd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	pfds.push_back(pfd);
}

// Remove an index from the set
void delFromPfds(std::vector<struct pollfd>& pfds, int i)
{
	pfds.erase(pfds.begin() + i);
}
