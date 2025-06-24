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
int receiveClientRequest(Connection &connection, std::vector<Server>& servers)
{
    std::string headerStr;
    std::string bodyStr;
    HttpRequest& request = connection.request;
    HttpResponse& response = connection.response;

    std::string choosePort = getSocketPortNumber(connection.fd);
    Server server = getServerByPort(servers, choosePort);

    int ret = 0;
    if (request.getMethod().empty()) {
        ret = readRequestHeader(connection, headerStr, server.getClientHeaderBufferSize());

        if (ret < 0)
            return ret;

        try {
            request.parseRequestLine(headerStr);
            request.parseHeaderLines(headerStr);
        }
        catch (const HttpException& e) {
            std::cerr << "Error: " <<  e.what() << "\n";
            response.setStatus(e.getStatusCode());

            // Add a better exception handler here for the error codes
            connection.connType = CLOSE;
            connection.isResponseReady = true;
            return 1; // Find a better way to exit the function and send the response
        }
    }

    // Refactor later
    response.setHeader("Connection", request.getHeader("Connection"));
    if (request.getHeader("Connection") == "close")
        connection.connType = CLOSE;

    std::cout << "header size: " << headerStr.size() << "\n"; ////debug

    Location location = server.getLocationPath(request.getURI());
    if (location.alias.empty() && location.root.empty())
    {
        response.setStatus(NOT_FOUND);
        response.setHeader("Content-Type", "text/plain");
        response.setBody("404 Not Found: The requested resource could not be found.\n");
    }

	// Initialise `readBodyMethod
	if (request.hasHeader("Content-Length"))
		connection.readBodyMethod = CONTENT_LENGTH;
	else if (request.hasHeader("Transfer-Encoding"))
		connection.readBodyMethod = CHUNKED_ENCODING;
	else
		connection.readBodyMethod = NO_BODY;

	if (connection.readBodyMethod != NO_BODY) {
		try {
			int ret2 = readRequestBody(connection, bodyStr, server.getClientBodyBufferSize());
			if (ret2 < 0)
				return ret2;

			std::cout << "\n===== body String: =====\n";
			std::cout << bodyStr << '\n';
			std::cout << "==========================\n\n";

			request.setBody(bodyStr);
		}
		catch (std::exception& e) {
			std::cerr << e.what() << "\n";
			connection.connType = CLOSE;
		}
	}

    // get the cgi path
    if (!location.cgi_path.empty())
    {
        std::cout << GREEN "CGI found\n" RESET; ////debug
        Cgi	cgi;
        std::string cgiOutput = cgi.executeCgi(request, response); //? handle by response
        std::cerr << "cgiOutput: " << cgiOutput << "\n";
        if (cgiOutput.empty() || cgiOutput == "")
        {
            response.setStatus(INTERNAL_ERROR);
            response.setHeader("Content-Type", "text/plain");
            response.setBody("500 Internal Server Error: CGI script execution failed.\n");
        }
        else
        {
            handleCgiRequest(cgiOutput, response);
            std::cout << "---------- CGI Output ----------\n" << BLUE << response.toString() << RESET << "\n";
        }
    }
    else if (response.getStatus() == OK && request.getMethod() == "GET")
        response.handleGetRequest(request, server);

    std::cout << request;
    connection.isResponseReady = true;
	connection.clearBuffer();

    return 0;
}

