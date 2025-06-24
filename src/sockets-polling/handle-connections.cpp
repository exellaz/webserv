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
    return 1;
}

void dispatchRequest(Connection& connection)
{
    HttpRequest& request = connection.request;
    HttpResponse& response = connection.response;
    const Location& location = connection.location;

    response.setHeader("Connection", request.getHeader("Connection"));
    if (request.getHeader("Connection") == "close")
        connection.connType = CLOSE;

    if (!location.cgi_path.empty()) {
        std::cout << GREEN "CGI found\n" RESET;
        Cgi cgi;
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
            response.handleGetRequest(request, connection.config);
        // else if (req.getMethod() == "POST")
        //     res.handlePostRequest(req, connection.config);
        // else if (req.getMethod() == "DELETE")
        //     res.handleDeleteRequest(req, connection.config);
    }
}


void Connection::resolveServerConfig(std::vector<Config>& configs, HttpRequest& request)
{
    std::string choosePort = request.getHeader("Host").substr(request.getHeader("Host").rfind(':') + 1);
    config = getServerConfigByPort(configs, choosePort);
    location = config.getLocationPath(request.getURI());
    if (location.alias.empty() && location.root.empty())
        throw HttpException(NOT_FOUND, "Requested resource not found");
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

/*
NOTE:
- `readHeader` will remove the 'header' section from `buffers`
- if recv(HEADER_BUFFER_SIZE) reads till the 'body' section, that section of 'body' will remain in buffers after `readHeader()` is called

*/
int receiveClientRequest(Connection &connection, std::vector<Config>& configs)
{
    HttpRequest& request = connection.request;
    HttpResponse& response = connection.response;

    if (!request.isHeaderParsed()) {
        std::string headerStr;
        int ret = readRequestHeader(connection, headerStr);
        if (ret < 0)
            return ret;

        try {
            request.parseRequestLine(headerStr);
            request.parseHeaderLines(headerStr);
        }
        catch (const HttpException& e) {
            return handleParsingError(e, response, connection);
        }
    }

    // Refactor later
    response.setHeader("Connection", request.getHeader("Connection"));
    if (request.getHeader("Connection") == "close")
        connection.connType = CLOSE;

    try {
        connection.resolveServerConfig(configs, request);
    }
    catch (const HttpException& e) {
        return handleParsingError(e, response, connection);
    }

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
            int ret2 = readRequestBody(connection, bodyStr);
            if (ret2 < 0)
                return ret2;
            request.setBody(bodyStr);
        }
        catch (const HttpException& e) {
            return handleParsingError(e, response, connection);
        }
    }
    return 0;
}

