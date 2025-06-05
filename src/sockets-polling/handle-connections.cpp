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
		"socket %d\n", inet_ntop(remoteAddr.ss_family,
			getInAddr((struct sockaddr*)&remoteAddr),
			remoteIp, INET6_ADDRSTRLEN),
		newFd);
}

/*
NOTE: 
- `readHeader` will remove the 'header' section from `buffers`
- if recv(HEADER_BUFFER_SIZE) reads till the 'body' section, that section of 'body' will remain in buffers after `readHeader()` is called

*/
int receiveClientRequest(int fd)
{
	// std::vector<Buffer> buffers;
	std::string buffer;	
	std::string headerStr;
	std::string bodyStr;
	
	readRequestHeader(fd, headerStr, buffer);
	// parseRequestHeader();
	
	// TODO: isBodyPresent()   -> check Content-Length, Transfer-Encoding, request method
	
	readRequestBody(fd, bodyStr, buffer, CONTENT_LENGTH); // hardcoded to 'CONTENT_LENGTH'
	// parseRequestBody();
    return 0;
}
