#include "../../include/sockets-polling.h"
#include "../../include/http-request.h"

void acceptClient(std::vector<struct pollfd>& pfds, std::vector<Connection>& connections, int listener)
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

	addToPfds(pfds, newFd);
	connections.push_back(Connection(newFd, getNowInSeconds()));

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
int receiveClientRequest(Connection &connection)
{
	std::string headerStr;
	std::string bodyStr;
	HttpRequest& request = connection.request;
	HttpResponse& response = connection.response;

	int ret = 0;
	if (request.getMethod().empty()) {
		ret = readRequestHeader(connection, headerStr);

		if (ret < 0)
			return ret;

		try {
			request.parseRequestLine(headerStr, response);
			request.parseHeaderLines(headerStr, response);

		}
		catch (std::exception &e) {
			std::cerr << e.what() << "\n";
			connection.connType = CLOSE;
		}
	}

	// parseRequestHeader();
	if (response.getStatus() == OK && request.getMethod() == "GET")
		response.handleGetRequest(request, ".");

	// TODO: isBodyPresent()   -> check Content-Length, Transfer-Encoding, request method
	if (!request.getHeader("Content-Length").empty())
		connection.readBodyMethod = CONTENT_LENGTH;
	else if (!request.getHeader("Transfer-Encoding").empty())
		connection.readBodyMethod = CHUNKED_ENCODING;
	else
		connection.readBodyMethod = NO_BODY;
	std::cout << "CHECK\n";	

	if (request.getHeaders().find("Content-Length") != request.getHeaders().end() || 
		request.getHeaders().find("Transfer-Encoding") != request.getHeaders().end()) {
		int ret2 = readRequestBody(connection, bodyStr);
		if (ret2 < 0)
			return ret2;
		request.setBody(bodyStr);
	}
	std::cout << request;
	connection.isResponseReady = true;
	// parseRequestBody();
	return 0;
}
