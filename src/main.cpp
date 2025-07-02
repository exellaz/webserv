#include "sockets-polling.h"
#include "Configuration.hpp"
#include "timeout.h"

int main(int argc, char **argv)
{
    std::string configFile;

    if (argc < 2) {
        std::cout << "No config file found. Using default config file.\n";
        (void)argv;
        configFile = "./conf/default.conf";
    }
    else if (argc > 2) {
        std::cout << "Error: Expected 1 config file only.\n";
        return 1;
    }
    else {
        configFile = argv[1];
    }

    try {
        std::map< std::pair<std::string, std::string> , std::vector<Server> > servers = parseAllServers(configFile);
        std::vector<struct pollfd> pfds;
        std::vector<int> listeners;
        std::vector<Client> clients;

        for (std::map<std::pair<std::string, std::string>, std::vector<Server> >::iterator it = servers.begin(); it != servers.end(); ++it)
        {
            Server &curServer = *(it->second.begin());
            setupListeningSocket(pfds, listeners, curServer);
        }
        for (std::vector<int>::iterator it = listeners.begin(); it != listeners.end(); ++it) ////debug
            std::cout << "Listener socket fd: " << *it << "\n";

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
                Client *client = findClientByFd(clients, pfds[i].fd);
                if (pfds[i].revents & POLLIN)
                    handlePollIn(servers, pfds[i], *client);
                else if (pfds[i].revents & POLLOUT)
                    handlePollOut(pfds[i], *client);
                else if (pfds[i].revents & POLLHUP)
                    handlePollHup(*client);
                else if (pfds[i].revents & POLLERR)
                    handlePollErr(*client);

            }
            clearDisconnectedClients(clients, pfds);
        }

    }
    catch (const std::exception &e)
    {
        std::cerr << RED << "Error: " << RESET << e.what() << "\n";
    }


    return 0;
}
