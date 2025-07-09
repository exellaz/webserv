#include "handle-sockets.h"

std::vector<Client>::iterator disconnectClient(std::vector<Client>& clients, std::vector<Client>::iterator &clientIt, std::vector<struct pollfd>& pfds)
{
    std::cout << RED << infoTime() << "server: disconnected client socket " << clientIt->getFd() << "\n" << RESET;

    // find corresponding pfd element based on fd
    std::vector<struct pollfd>::iterator pfdIt = pfds.begin();
    for (; pfdIt != pfds.end(); ++pfdIt) {
        if (pfdIt->fd == clientIt->getFd())
            break;
    }

    close(clientIt->getFd());
    pfds.erase(pfdIt);
    std::vector<Client>::iterator nextClientIt = clients.erase(clientIt);

    return nextClientIt;
}
