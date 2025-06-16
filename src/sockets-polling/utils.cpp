#include "../../include/sockets-polling.h"


// Get sockaddr, IPv4 or IPv6:
void *getInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Helper: set a file descriptor to non-blocking mode
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) 
		return -1;
	// set file status with existing flags + Non-blocking flag
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
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

void disconnectClient(std::vector<Connection>& connections, std::vector<struct pollfd>& pfds, int index)
{
	std::cout << RED << "server: disconnected client socket " << connections[index].fd << "\n" << RESET << '\n';
	close(connections[index].fd);

	connections.erase(connections.begin() + index);
	pfds.erase(pfds.begin() + index);
}



time_t getNowInSeconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}
