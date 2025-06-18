#include "../include/sockets-polling.h"
#include "../include/Configuration.hpp"
#include "../include/http-request.h"
#include "../include/http-response.h"

void sendResponseToClient(int fd, HttpResponse& response)
{
    send(fd, response.toString().c_str(), response.toString().size(), 0);
    std::cout << "server: Response sent to client.\n";
}

int main(int argc, char **argv)
{
    std::string configFile;

    if (argc < 2) {
        std::cout << "No config file found. Using default config file.\n";
        (void)argv;
        configFile = "./src/config-parser/server.conf";
    }
    else if (argc > 2) {
        std::cout << "Error: Expected 1 config file only.\n";
        return 1;
    }
    else {
        configFile = argv[1];
    }

    try { std::ifstream conf(configFile.c_str()); Config config(conf);

        std::vector<struct pollfd> pfds;
        std::vector<Connection> connections;

        // listener Socket Fd
        int listener = setupListeningSocket(pfds, connections, config);

        while(1) {

            std::cout << CYAN << "\n+++++++ Waiting for new connection ++++++++" << RESET << "\n\n";

			// TODO: calculate nearest Timeout among all connections, assign to poll()

            // wait until 1 or more fds become ready for reading (POLLIN) or other events.
			int nearestTimeout = getNearestUpcomingTimeout(connections);
            int pollCount = poll(&pfds[0], pfds.size(), nearestTimeout);
            if (pollCount == -1) {
                throw PollErrorException();
            }

			disconnectTimedOutClients(connections, pfds);

            // Run through the existing connections looking for data to read
            for(size_t i = 0; i < pfds.size(); i++) {
                // Check if socket is ready to read
                if (pfds[i].revents & (POLLIN )) {
                    std::cout << "POLLIN\n";

                    if (pfds[i].fd == listener)
                        acceptClient(pfds, connections, listener);
                    else {
						int res = receiveClientRequest(connections[i]);
						if (res == RECV_CLOSED) {
							std::cout << "server: socket " << pfds[i].fd << " hung up\n";
							disconnectClient(connections, pfds, i);
							i--;
							continue;
						}
						// connections[i].isResponseReady = true; // HARDCODED
						if (connections[i].isResponseReady)
							pfds[i].events |= POLLOUT;
                    }
                }
                else if (pfds[i].revents & POLLOUT) {
                    std::cout << "POLLOUT\n";

                    sendResponseToClient(connections[i].fd, connections[i].response);
                    // pfds[i].events |= POLLIN;
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
