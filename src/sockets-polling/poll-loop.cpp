#include "../../include/sockets-polling.h"
#include "timeout.h"


void pollLoop(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
              std::vector<struct pollfd>& pfds,
              std::vector<int>& listeners,
              std::vector<Client>& clients)
{
    while(true) {
        std::cout << CYAN << "\n+++++++ Waiting for POLL event ++++++++" << RESET << "\n\n";

        int nearestTimeout = getNearestUpcomingTimeout(clients, servers);
        int pollCount = poll(&pfds[0], pfds.size(), nearestTimeout);
        if (pollCount == -1)
            throw PollErrorException();

        disconnectTimedOutClients(clients, pfds, servers);

        for(size_t i = 0; i < pfds.size(); i++) {
            if (pfds[i].revents == 0)
                continue;
            // check for Incoming Client Connection
            if (isListener(listeners, pfds[i].fd) && pfds[i].revents & POLLIN) {
                acceptClient(pfds, clients, pfds[i].fd);
                continue;
            }
            // check for Client Poll Event
            Client& client = findClientByFd(clients, pfds[i].fd);
            if (pfds[i].revents & POLLIN)
                handlePollIn(servers, pfds[i], client);
            else if (pfds[i].revents & POLLOUT)
                handlePollOut(pfds[i], client);
            else if (pfds[i].revents & POLLHUP)
                handlePollHup(client);
            else if (pfds[i].revents & POLLERR)
                handlePollErr(client);

        }
        clearDisconnectedClients(clients, pfds);
    }
}