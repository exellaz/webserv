#include "../include/sockets-polling.h"
#include "../include/Configuration.hpp"


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
            
            std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n";
            // wait until 1 or more fds become ready for reading (POLLIN) or other events.
            int pollCount = poll(&pfds[0], pfds.size(), -1);
            if (pollCount == -1) {
				throw PollErrorException();
            }
            // Run through the existing connections looking for data to read
            for(size_t i = 0; i < pfds.size(); i++) {


				// Check if socket is ready to read
                if (pfds[i].revents & (POLLIN )) {
                    std::cout << "POLLIN\n";

                    if (pfds[i].fd == listener)
                        acceptClient(pfds, connections, listener, i);
                    else { 
						// receiveClientRequest(pfds[i].fd);
						receiveClientRequest(connections[i]);
                    }
                } else if (pfds[i].revents & POLLOUT) {
					std::cout << "POLLOUT\n";
				} else if (pfds[i].revents & (POLLHUP)) {
					std::cout << "POLLHUP\n";
					close(pfds[i].fd);
					delFromPfds(pfds, i);
					i--;
				} else if (pfds[i].revents & (POLLERR)) {
					std::cout << "POLLERR\n";
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
