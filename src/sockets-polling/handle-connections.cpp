#include "../../include/sockets-polling.h"
#include "../../include/http-request.h"

int handleParsingError(const HttpException& e, HttpResponse& response, Connection& connection)
{
    std::cerr << "HTTP Error: " << e.getStatusCode() << " - " << e.what() << "\n";
    response.setStatus(e.getStatusCode());
    // Set body here being the respective error page
    response.setHeader("Connection", "close");
    connection.connType = CLOSE;
    connection.isResponseReady = true;
    return -3;
}

void dispatchRequest(Connection& connection)
{
    HttpRequest& request = connection.request;
    HttpResponse& response = connection.response;

    response.setHeader("Connection", request.getHeader("Connection"));
    if (request.getHeader("Connection") == "close")
        connection.connType = CLOSE;

    if (!connection.location.cgi_path.empty()) {
        std::cout << GREEN "CGI found\n" RESET;
        Cgi cgi;

        std::cout << "response body: \n" << response.getBody() << '\n';
        std::string cgiOutput = cgi.executeCgi(request, response);
        std::cerr << "cgiOutput: " << cgiOutput << "\n";

        if (cgiOutput.empty())
            throw HttpException(INTERNAL_ERROR, "CGI script execution failed");
        else {
            handleCgiRequest(cgiOutput, response);
            std::cout << "---------- CGI Output ----------\n" << BLUE << response.toString() << RESET << "\n";
        }
    }
    else {
        if (request.getMethod() == "GET")
            response.handleGetRequest(request.getURI(), connection.server, connection.location);
        else if (request.getMethod() == "POST")
            response.handlePostRequest(request, "/objs/"); // objs hardcoded, change to full path without index
        else if (request.getMethod() == "DELETE")
            throw HttpException(METHOD_NOT_ALLOWED, "Delete without CGI not allowed");
    }
}

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

void validateMethod(const std::string& method, const std::vector<std::string>& allowedMethods)
{
    for (std::vector<std::string>::const_iterator It = allowedMethods.begin(); It != allowedMethods.end(); ++It) {

        if (*It == method)
            return ;
    }
    throw HttpException(METHOD_NOT_ALLOWED, "Method not allowed");
}




/*
NOTE:
- `readHeader` will remove the 'header' section from `buffers`
- if recv(HEADER_BUFFER_SIZE) reads till the 'body' section, that section of 'body' will remain in buffers after `readHeader()` is called

*/
int receiveClientRequest(Connection &connection, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers)
{
    HttpRequest& request = connection.request;
    HttpResponse& response = connection.response;

    // IP:PORT pair from fd
    std::pair<std::string, std::string> ipPort = getIpAndPortFromSocketFd(connection.fd);

    // get default block by IP:PORT pair
    Server& defaultServer = getDefaultServerBlockByIpPort(ipPort, servers);

    if (!request.isHeaderParsed()) {
        try {
            std::string headerStr;
            int ret = readRequestHeader(connection, headerStr, defaultServer.getClientHeaderBufferSize());
            if (ret < 0)
                return ret;
            request.parseRequestLine(headerStr);
            request.parseHeaderLines(headerStr);

            connection.assignServerByServerName(servers, ipPort, defaultServer);
            connection.location = connection.server.getLocationPath(request.getURI());
            std::cout << connection.location.allowMethods.size() << "\n";
            validateMethod(request.getMethod(), connection.location.allowMethods);
        }
        catch (const HttpException& e) {
            return handleParsingError(e, response, connection);
        }
    }

    // Refactor later
    response.setHeader("Connection", request.getHeader("Connection"));
    if (request.getHeader("Connection") == "close")
        connection.connType = CLOSE;

    // Initialise `readBodyMethod`
    if (request.hasHeader("Content-Length"))
        connection.readBodyMethod = CONTENT_LENGTH;
    else if (request.hasHeader("Transfer-Encoding"))
        connection.readBodyMethod = CHUNKED_ENCODING;
    else
        connection.readBodyMethod = NO_BODY;

    if (connection.readBodyMethod != NO_BODY) {
        try {
            std::string bodyStr;
            int ret2 = readRequestBody(connection, bodyStr, defaultServer.getClientBodyBufferSize());
            if (ret2 < 0)
                return ret2;
            request.setBody(bodyStr);
        }
        catch (const HttpException& e) {
            return handleParsingError(e, response, connection);
        }
    }

    std::cout << request;
    return 0;
}

