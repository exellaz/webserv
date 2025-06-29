#include "../include/sockets-polling.h"
#include "../include/Configuration.hpp"
#include "../include/http-request.h"
#include "../include/http-response.h"
#include <algorithm>

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
        std::vector<Connection> connections;
        std::vector<int> listeners;

        for (std::map<std::pair<std::string, std::string>, std::vector<Server> >::iterator it = servers.begin(); it != servers.end(); ++it)
        {
            Server &curServer = *(it->second.begin());
            int listener = setupListeningSocket(pfds, connections, curServer); //? set map in this
            listeners.push_back(listener);
        }
        for (std::vector<int>::iterator it = listeners.begin(); it != listeners.end(); ++it) ////debug
            std::cout << "Listener socket fd: " << *it << "\n";

        while(1) {
            std::cout << CYAN << "\n+++++++ Waiting for POLL event ++++++++" << RESET << "\n\n";

            int nearestTimeout = getNearestUpcomingTimeout(connections, listeners.size(), servers);
            int pollCount = poll(&pfds[0], pfds.size(), nearestTimeout);
            if (pollCount == -1)
                throw PollErrorException();

            disconnectTimedOutClients(connections, pfds, listeners.size(), servers);

            // Run through the existing connections looking for data to read
            for(size_t i = 0; i < pfds.size(); i++) {
                // Check if socket is ready to read
                if (pfds[i].revents & (POLLIN ))
                    handlePollIn(servers, pfds, connections, listeners, i);
                else if (pfds[i].revents & POLLOUT)
                    handlePollOut(pfds, connections, i);
                else if (pfds[i].revents & POLLHUP)
                    handlePollHup(connections, i);
                else if (pfds[i].revents & POLLERR)
                    handlePollErr(connections, i);

                // erase DISCONNECTED client from `pfds` & `Connections`
                for(size_t i = 0; i < pfds.size(); i++) {
                    if (connections[i].connState == DISCONNECTED)
                        disconnectClient(connections, pfds, i--);
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
