#include "../include/sockets-polling.h"
#include "../include/Configuration.hpp"
#include "../include/http-request.h"
#include "../include/http-response.h"
#include <algorithm>

void sendResponseToClient(int fd, HttpResponse& response)
{
    // // HTML body
    // const std::string html_body =
    //     "<!DOCTYPE html>\r\n"
    //     "<html lang=\"en\">\r\n"
    //     "<head>\r\n"
    //     "  <meta charset=\"UTF-8\">\r\n"
    //     "  <title>Sample HTTP Response</title>\r\n"
    //     "</head>\r\n"
    //     "<body>\r\n"
    //     "  <h1>Hello, World!</h1>\r\n"
    //     "  <p>This is a sample HTTP response with Content-Length header.</p>\r\n"
    //     "</body>\r\n"
    //     "</html>\r\n";
    // // Calculate content length
    // std::size_t content_length = html_body.size();
    // // Convert content_length to string
    // std::stringstream ss;
    // ss << content_length;
    // std::string content_length_str = ss.str();
    // // Entire HTTP response in one string
    // std::string response =
    //     "HTTP/1.1 200 OK\r\n"
    //     "Content-Type: text/html; charset=UTF-8\r\n"
    //     "Content-Length: " + content_length_str + "\r\n"
    //     "Connection: close\r\n"
    //     "\r\n" +
    //     html_body;
    // send(fd, &response[0], response.size(), 0);

    send(fd, response.toString().c_str(), response.toString().size(), 0);
    std::cout << "server: Response sent to client.\n";
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
		std::vector<Config> configs = parseAllServers(configFile);

        std::vector<struct pollfd> pfds;
	    std::vector<Connection> connections;
		std::vector<int> listeners;
		std::map<int, std::string> listenerToConfig;

        // listener Socket Fd
        //int listener = setupListeningSocket(pfds, connections, configs);
		for (size_t i = 0; i < configs.size(); ++i)
		{
			if (configs[i].getPort().empty() || configs[i].getHost().empty())
				continue;
			int listener = setupListeningSocket(pfds, connections, configs[i]);
			listeners.push_back(listener);
		}

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

					//look for multiple listeners
                    if (find(listeners.begin(), listeners.end(), pfds[i].fd) != listeners.end())
                        acceptClient(pfds, connections, pfds[i].fd);
                    else {
						//int res = receiveClientRequest(connections[i]);
						int res = receiveClientRequest(connections[i], configs); //pass multiple configs
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
