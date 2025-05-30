#include "sockets-polling.h"

void acceptClient(std::vector<struct pollfd>& pfds, int listener)
{
	// If listener is ready to read, handle new connection
	struct sockaddr_storage remoteAddr; // Client address
	socklen_t addrLen = sizeof remoteAddr;
	int newFd = accept(listener, (struct sockaddr *)&remoteAddr, &addrLen);
	if (newFd == -1)
		perror("accept");
	else {
		
		// server sends text to every client that connects
		const char *hello = "Hello from server!\n";
		send(newFd, hello, strlen(hello), 0);

		addToPfds(pfds, newFd);

		char remoteIp[INET6_ADDRSTRLEN];
		printf("pollserver: new connection from %s on "
			"socket %d\n",
			inet_ntop(remoteAddr.ss_family,
				getInAddr((struct sockaddr*)&remoteAddr),
				remoteIp, INET6_ADDRSTRLEN),
			newFd);
	}
}

void receiveClientData(struct pollfd& pfd)
{
	char buf[3000];    // Buffer for client data
	int nBytes = recv(pfd.fd, buf, sizeof buf, 0);
	buf[nBytes] = '\0';
	int senderFd = pfd.fd;

	if (nBytes <= 0) {
		// Got error or connection closed by client
		if (nBytes == 0) {
			// Connection closed
			std::cout << "pollserver: socket " << senderFd << " hung up\n"; 
		} else {
			perror("recv");
		}
	} else {
		// do something with client data
		std::cout << "Data from Client: \n"<< buf << '\n';

		const std::string response = "Response from server\n";
		send(pfd.fd, response.c_str(), response.size(), 0);
	}
}


// Main
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
            // Check if someone's ready to read
            if (pfds[i].revents & (POLLIN)) { // We got one!!
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
