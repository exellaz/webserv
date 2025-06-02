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


// int readRequestHeader(int fd, std::string& headerStr)
// {
//     char buf[HEADER_BUFFER_SIZE + 1]; 
//
//     while (true) {
//         int nBytes = recv(fd, buf, HEADER_BUFFER_SIZE, 0);
//         if (nBytes == -1) { // Error receiving data
// 			std::cout << "recv: no data available in socket\n";
//     		std::cout << "\nData from Client: \n" << headerStr << '\n';
//
//             return -1;
//         } else if (nBytes == 0) {
//             // Client connection closed
//             std::cout << "recv: socket " << fd << " hung up\n"; 
//     		std::cout << "\nData from Client: \n" << headerStr << '\n';
//
//             return -2;
//         }
//         buf[nBytes] = '\0'; // Null-terminate the received data
//         headerStr += buf;  // Append to the string
//     }
//
//     std::cout << "\nData from Client: \n" << headerStr << '\n';
//     const std::string response = "Response from server\n";
//     send(fd, response.c_str(), response.size(), 0);
//
// }

int readRequestBody(int fd, std::string& bodyStr, int contentLength)
{
	(void)contentLength;

    char buf[BODY_BUFFER_SIZE + 1]; 
	
    while (true) {
        int nBytes = recv(fd, buf, BODY_BUFFER_SIZE, 0);
        if (nBytes == -1) { // Error receiving data
			std::cout << "recv: no data available in socket\n";
    		std::cout << "\nData from Client: \n" << bodyStr << '\n';

            return -1;
        } else if (nBytes == 0) {
            // Client connection closed
            std::cout << "recv: socket " << fd << " hung up\n"; 
    		std::cout << "\nData from Client: \n" << bodyStr << '\n';

            return -2;
        }
        buf[nBytes] = '\0'; // Null-terminate the received data
        bodyStr += buf;  // Append to the string
    }

    std::cout << "Data from Client: \n" << bodyStr << '\n';
    const std::string response = "Response from server\n";
    send(fd, response.c_str(), response.size(), 0);

}

/*
NOTE: 
- `readHeader` will remove the 'header' section from `buffers`
- if recv(HEADER_BUFFER_SIZE) reads till the 'body' section, that section of 'body' will remain in buffers after `readHeader()` is called

*/
int receiveClientRequest(int fd)
{
	std::vector<Buffer> buffers;
	std::string headerStr;
	std::string bodyStr;
	
	readRequestHeader(fd, headerStr, buffers);
	// parseRequestHeader();
	readRequestBody(fd, bodyStr, 0);
	// parseRequestBody();

    return 0;
}
