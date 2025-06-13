#include "../../include/sockets-polling.h"
#include "../../include/http-request.h"

void acceptClient(std::vector<struct pollfd>& pfds, std::vector<Connection>& connections, int listener, int index)
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
	// const char *hello = "Hello from server!\n";
	// send(newFd, hello, strlen(hello), 0);

	addToPfds(pfds, newFd);
	connections.push_back(Connection(index, newFd));

	char remoteIp[INET6_ADDRSTRLEN];
	printf("server: new connection from %s on "
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
int receiveClientRequest(Connection &connection, HttpRequest& request, HttpResponse& response)
{
	std::string headerStr;
	std::string bodyStr;

	int ret = readRequestHeader(connection, headerStr);
	if (ret < 0)
		return ret;
	// parseRequestHeader();
	request.parseRequestLine(headerStr);
	request.parseHeaderLines(headerStr, response);
	request.setBody(bodyStr);

	if (request.getMethod() == "GET")
		response.handleGetRequest(request, ".");

	std::cout << request;
	connection.isResponseReady = true;

	// TODO: isBodyPresent()   -> check Content-Length, Transfer-Encoding, request method
	// ret = readRequestBody(connection, bodyStr);
	// if (ret < 0)
	// 	return ret;

	// parseRequestBody();
	return 0;
}

