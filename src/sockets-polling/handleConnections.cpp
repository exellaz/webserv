#include "../../include/sockets-polling.h"

void acceptClient(std::vector<struct pollfd>& pfds, int listener)
{
	// If listener is ready to read, handle new connection
	struct sockaddr_storage remoteAddr; // Client address
	socklen_t addrLen = sizeof remoteAddr;
	int newFd = accept(listener, (struct sockaddr *)&remoteAddr, &addrLen);
	if (newFd == -1) {
		perror("accept");
		return ;
	}
	if (set_nonblocking(newFd) == -1) {
		perror("set_nonblocking (new_fd)");
		close(newFd);
		return ;
	}
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

void receiveClientData(int fd)
{
	// TODO: create loop to recv() in chunks
	char buf[3000];    // Buffer for client data
	int nBytes = recv(fd, buf, sizeof buf, 0);
	buf[nBytes] = '\0';

	if (nBytes <= 0) {
		// Got error or connection closed by client
		if (nBytes == 0) {
			// Connection closed
			std::cout << "pollserver: socket " << fd << " hung up\n"; 
		} else {
			perror("recv");
		}
	} else {
		// do something with client data
		std::cout << "Data from Client: \n"<< buf << '\n';

		const std::string response = "Response from server\n";
		send(fd, response.c_str(), response.size(), 0);
	}
}
