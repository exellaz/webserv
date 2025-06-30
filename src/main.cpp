#include "../include/sockets-polling.h"
#include "../include/Configuration.hpp"
#include "../include/http-request.h"
#include "../include/http-response.h"
#include <algorithm>
#include <map>

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
            int listener = setupListeningSocket(pfds, clients, curServer); //? set map in this
            listeners.push_back(listener);
        }
        for (std::vector<int>::iterator it = listeners.begin(); it != listeners.end(); ++it) ////debug
            std::cout << "Listener socket fd: " << *it << "\n";

        while(1) {
            std::cout << CYAN << "\n+++++++ Waiting for POLL event ++++++++" << RESET << "\n\n";

            int nearestTimeout = getNearestUpcomingTimeout(clients, listeners.size(), servers);
            int pollCount = poll(&pfds[0], pfds.size(), nearestTimeout);
            if (pollCount == -1)
                throw PollErrorException();

            disconnectTimedOutClients(clients, pfds, listeners.size(), servers);

            // Run through the existing clients looking for data to read
            for(size_t i = 0; i < pfds.size(); i++) {
                // Check if socket is ready to read
                if (pfds[i].revents & (POLLIN ))
                    handlePollIn(servers, pfds, clients, listeners, i);
                else if (pfds[i].revents & POLLOUT)
                    handlePollOut(pfds, clients, i);
                else if (pfds[i].revents & POLLHUP)
                    handlePollHup(clients, i);
                else if (pfds[i].revents & POLLERR)
                    handlePollErr(clients, i);

                // erase DISCONNECTED client from `pfds` & `Connections`
                for(size_t i = 0; i < pfds.size(); i++) {
                    if (clients[i].connState == DISCONNECTED)
                        disconnectClient(clients, pfds, i--);
                }
            }
        }

    }
    catch (const std::exception &e)
    {
        std::cerr << RED << "Error: " << RESET << e.what() << "\n";
    }


    return 0;
}
