#include <algorithm> // for isListener
#include "poll-loop.h"

bool isListener(std::vector<int>& listeners, int fd)
{
    if (find(listeners.begin(), listeners.end(), fd) != listeners.end())
        return true;
    return false;
}

Client& findClientByFd(std::vector<Client>& clients, int fd)
{
    std::vector<Client>::iterator it = clients.begin();
    for (; it != clients.end(); ++it) {
        if (it->getFd() == fd)
            return (*it);
    }
    throw std::runtime_error("Error: findClientByFd(): Client not found for given fd");
}

void clearDisconnectedClients(std::vector<Client>& clients, std::vector<struct pollfd>& pfds)
{
    std::vector<Client>::iterator clientIt = clients.begin();
    for(; clientIt != clients.end();) {
        if (clientIt->getConnState() == DISCONNECTED)
            clientIt = disconnectClient(clients, clientIt, pfds);
        else
            ++clientIt;
    }
}