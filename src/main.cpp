#include "../include/sockets-polling.h"
#include "../include/Configuration.hpp"
#include "../include/http-request.h"
#include "../include/http-response.h"
#include <algorithm>

void sendResponseToClient(int fd, HttpResponse& response)
{
    send(fd, response.toString().c_str(), response.toString().size(), 0);
    std::cout << "server: Response sent to client.\n";
}

/**
 * @brief check the pdfs.fd is similar to the connection.fd
*/
std::vector<Connection>::iterator CheckConnection(std::vector<Connection> &connections, int fd)
{
    for (std::vector<Connection>::iterator it = connections.begin(); it != connections.end(); ++it) {
        if (it->fd == fd)
            return it;
    }
    return connections.end();
}

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
        std::vector<Server> servers = parseAllServers(configFile);

        std::vector<struct pollfd> pfds;
        std::vector<Connection> connections;
        std::vector<int> listeners;
        std::map<int, std::string> listenerToConfig;

        for (size_t i = 0; i < servers.size(); ++i)
        {
            if (servers[i].getPort().empty() || servers[i].getHost().empty())
                continue;
            int listener = setupListeningSocket(pfds, connections, servers[i]);
            listeners.push_back(listener);
        }
        for (std::vector<int>::iterator it = listeners.begin(); it != listeners.end(); ++it) { ////debug
            std::cout << "Listener socket fd: " << *it << "\n";
        }

        while(1) {

            std::cout << CYAN << "\n+++++++ Waiting for POLL event ++++++++" << RESET << "\n\n";

            // wait until 1 or more fds become ready for reading (POLLIN) or other events.
            int nearestTimeout = getNearestUpcomingTimeout(connections, listeners.size());
            int pollCount = poll(&pfds[0], pfds.size(), nearestTimeout);
            if (pollCount == -1) {
                throw PollErrorException();
            }

            disconnectTimedOutClients(connections, pfds, listeners.size());

            // Run through the existing connections looking for data to read
            for(size_t i = 0; i < pfds.size(); i++) {
                // Check if socket is ready to read
                if (pfds[i].revents & (POLLIN )) {
                    std::cout << "POLLIN\n";

                    // If not a listener socket, then it's a client socket

                    //look for multiple listeners
                    if (find(listeners.begin(), listeners.end(), pfds[i].fd) != listeners.end())
                        acceptClient(pfds, connections, pfds[i].fd);
                    else {
                        int res = receiveClientRequest(connections[i], servers); //pass multiple servers
                        if (res == RECV_CLOSED) {
                            std::cout << "server: socket " << pfds[i].fd << " hung up\n";
                            disconnectClient(connections, pfds, i);
                            i--;
                            continue;
                        }
                        else if (res == RECV_AGAIN)
                            continue;
                        else if (res != -3) {
                            try {
                                dispatchRequest(connections[i]);
                            }
                            catch (const HttpException& e) {
                                handleParsingError(e, connections[i].response, connections[i]);
                            }
                        }
                        std::cout << "Response ready\n";
                        connections[i].isResponseReady = true;
                        connections[i].clearBuffer();
                        pfds[i].events |= POLLOUT;
                    }
                }
                else if (pfds[i].revents & POLLOUT) {
                    std::cout << "POLLOUT\n";

                    sendResponseToClient(connections[i].fd, connections[i].response);
                    // connections[i].location;
                    connections[i].isResponseReady = false;
                    connections[i].request.clearRequest();
                    connections[i].response.clearResponse();

                    if (connections[i].connType == CLOSE) {
                       disconnectClient(connections, pfds, i);
                       i--;
                       continue;
                    }
                    pfds[i].events &= ~POLLOUT;
                }
                else if (pfds[i].revents & POLLHUP) {
                    std::cout << "POLLHUP\n";
                    disconnectClient(connections, pfds, i);
                    i--;
                }
                else if (pfds[i].revents & POLLERR) {
                    std::cout << "POLLERR\n";
                    disconnectClient(connections, pfds, i);
                    i--;
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
