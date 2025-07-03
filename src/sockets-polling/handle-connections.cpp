#include "../../include/sockets-polling.h"
#include "../../include/http-request.h"

int handleParsingError(const HttpException& e, HttpResponse& response, Client& client)
{
    std::cerr << "HTTP Error: " << e.getStatusCode() << " - " << e.what() << "\n";
    response.setStatus(e.getStatusCode());
    // Set body here being the respective error page
    response.setHeader("Connection", "close");
    client.connType = CLOSE;
    client.isResponseReady = true;
    return REQUEST_ERR;
}

void dispatchRequest(Client& client)
{
    HttpRequest& request = client.request;
    HttpResponse& response = client.response;

    response.setHeader("Connection", request.getHeader("Connection"));
    if (request.getHeader("Connection") == "close")
        client.connType = CLOSE;
    if (!client.location.getReturnPath().empty())
    {
        int statusCode = client.location.getReturnPath().begin()->first;
        std::string returnPath = client.location.getReturnPath().begin()->second;
        if (statusCode == MOVED_PERMANENTLY || statusCode == FOUND)
        {
            client.response.setStatus(static_cast<HttpCodes::StatusCode>(statusCode));
            client.response.setHeader("Location", returnPath);
            client.response.setBody("");
        }
        else if (statusCode == OK)
        {
            client.response.setStatus(static_cast<HttpCodes::StatusCode>(statusCode));
            client.response.setHeader("Content-Type", "text/plain");
            client.response.setBody(returnPath);

        }
    }
    else if (client.location.getCgiPath() == true)
    {
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
        {
            std::cout << GREEN "GET request\n" RESET;
            response.handleGetRequest(client.location, client);
        }
        else if (request.getMethod() == "POST")
        {
            std::cout << GREEN "POST request\n" RESET;
            response.handlePostRequest(request, client);
        }
        else if (request.getMethod() == "DELETE")
            throw HttpException(METHOD_NOT_ALLOWED, "Delete without CGI not allowed");
    }
}

void acceptClient(std::vector<struct pollfd>& pfds, std::vector<Client>& clients, int listener)
{
    std::cout << "POLLIN: socket " << listener << '\n';

    // If listener is ready to read, handle new client
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
    clients.push_back(Client(newFd, getNowInSeconds()));

    char remoteIp[INET6_ADDRSTRLEN];
    printf("server: new client from %s on "
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
int receiveClientRequest(Client& client, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers)
{
    HttpRequest& request = client.request;
    HttpResponse& response = client.response;

    // IP:PORT pair from fd
    std::cout << "NULLL\n";
    std::pair<std::string, std::string> ipPort = getIpAndPortFromSocketFd(client.fd);

    // get default block by IP:PORT pair
    Server& defaultServer = getDefaultServerBlockByIpPort(ipPort, servers);

    if (!request.isHeaderParsed()) {
        try {
            std::string headerStr;
            int ret = readRequestHeader(client, headerStr, defaultServer.getClientHeaderBufferSize());
            if (ret < 0)
                return ret;
            request.parseRequestLine(headerStr);
            request.parseHeaderLines(headerStr);

            client.assignServerByServerName(servers, ipPort, defaultServer);
            client.location = client.server.getLocationPath(request.getURI());
            std::cout << "Connection Location Path: " << client.location.getLocaPath() << "\n";
            std::cout << "METHOD SIZE: " << client.location.getAllowMethods().size() << "\n"; //// debug
            validateMethod(request.getMethod(), client.location.getAllowMethods());
        }
        catch (const HttpException& e) {
            return handleParsingError(e, response, client);
        }
    }

    // Refactor later
    response.setHeader("Connection", request.getHeader("Connection"));
    if (request.getHeader("Connection") == "close")
        client.connType = CLOSE;

    // Initialise `readBodyMethod`
    if (request.hasHeader("Content-Length"))
        client.readBodyMethod = CONTENT_LENGTH;
    else if (request.hasHeader("Transfer-Encoding"))
        client.readBodyMethod = CHUNKED_ENCODING;
    else
        client.readBodyMethod = NO_BODY;

    if (client.readBodyMethod != NO_BODY) {
        try {
            std::string bodyStr;

            int ret2 = readRequestBody(client, bodyStr, defaultServer.getClientBodyBufferSize(), defaultServer.getClientMaxSize());
            if (ret2 < 0)
                return ret2;
            request.setBody(bodyStr);
        }
        catch (const HttpException& e) {
            return handleParsingError(e, response, client);
        }
    }

    // std::cout << request;
    return RECV_OK;
}

