#include "sockets-polling.h"

int main(void)
{
	std::vector<struct pollfd> pfds;
	
	// listener Socket Fd
	int listener = setupListeningSocket(pfds);

    // Main loop
    while(1) {
		
		std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n";
        // wait until 1 or more fds become ready for reading (POLLIN) or other events.
        int pollCount = poll(&pfds[0], pfds.size(), -1);
        if (pollCount == -1) {
            perror("poll");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for(size_t i = 0; i < pfds.size(); i++) {

			if (pfds[i].revents & POLLHUP) { 
				std::cout << "POLLHUP\n";
			}
            // Check if socket is ready to read
            if (pfds[i].revents & (POLLIN)) { 
				std::cout << "POLLIN\n";

                if (pfds[i].fd == listener)
					acceptClient(pfds, listener);
                else { // Client socketfd
					receiveClientData(pfds[i]);
					close(pfds[i].fd);
					delFromPfds(pfds, i);
					i--;
                } 
            } 
        } 
    } 
    
    return 0;
}
