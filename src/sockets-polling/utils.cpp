#include "../../include/sockets-polling.h"

// Helper: set a file descriptor to non-blocking mode
int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    // set file status with existing flags + Non-blocking flag
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

std::vector<Client>::iterator disconnectClient(std::vector<Client>& clients, std::vector<Client>::iterator &clientIt, std::vector<struct pollfd>& pfds)
{
    std::cout << RED << "server: disconnected client socket " << clientIt->fd << "\n" << RESET << '\n';

    // find corresponding pfd element based on fd
    std::vector<struct pollfd>::iterator pfdIt = pfds.begin();
    for (; pfdIt != pfds.end(); ++pfdIt) {
        if (pfdIt->fd == clientIt->fd)
            break;
    }

    close(clientIt->fd);
    pfds.erase(pfdIt);
    std::vector<Client>::iterator nextClientIt = clients.erase(clientIt);

    return nextClientIt;
}




bool isListener(std::vector<int>& listeners, int fd)
{
    if (find(listeners.begin(), listeners.end(), fd) != listeners.end())
        return true;
    return false;
}

Client* findClientByFd(std::vector<Client>& clients, int fd)
{
    std::vector<Client>::iterator it = clients.begin();
    for (; it != clients.end(); ++it) {
        if (it->fd == fd)
            return &(*it);
    }
    std::cout << "findClientByFd: fd is not found among clients\n";
    return NULL;
}

void clearDisconnectedClients(std::vector<Client>& clients, std::vector<struct pollfd>& pfds)
{
    std::vector<Client>::iterator clientIt = clients.begin();
    for(; clientIt != clients.end();) {
        if (clientIt->connState == DISCONNECTED)
            clientIt = disconnectClient(clients, clientIt, pfds);
        else
            ++clientIt;
    }
}