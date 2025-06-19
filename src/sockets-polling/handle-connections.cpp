#include "../../include/sockets-polling.h"
#include "../../include/http-request.h"

void acceptClient(std::vector<struct pollfd>& pfds, std::vector<Connection>& connections, int listener)
{
    // If listener is ready to read, handle new connection
    struct sockaddr_storage remoteAddr; // Client address
    socklen_t addrLen = sizeof(remoteAddr);
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

    // Refactor later
    if (request.getHeader("Connection") == "close")
        connection.connType = CLOSE;

    std::cout << "header size: " << headerStr.size() << "\n"; ////debug

    std::string choosePort = request.getHeader("Host").substr(request.getHeader("Host").rfind(':') + 1);
    Config serverConfig = getServerConfigByPort(configs, choosePort);
    Location location = serverConfig.getLocationPath(request.getURI());
    if (location.alias.empty() && location.root.empty())
    {
        response.setStatus(NOT_FOUND);
        response.setHeader("Content-Type", "text/plain");
        response.setBody("404 Not Found: The requested resource could not be found.\n");
    }


    //check for body to handle
    if (request.getHeaders().find("Content-Length") != request.getHeaders().end()) {
        int ret2 = readRequestBody(connection, bodyStr);
        if (ret2 < 0)
            return ret2;
        std::cout << "Size of body: " << bodyStr.size() << "\n"; ////debug
        request.setBody(bodyStr);
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


    // parseRequestHeader();
    else if (response.getStatus() == OK && request.getMethod() == "GET")
        response.handleGetRequest(request, serverConfig);


    // TODO: isBodyPresent()   -> check Content-Length, Transfer-Encoding, request method

    //if (request.getHeaders().find("Content-Length") != request.getHeaders().end()) {
    //    int ret2 = readRequestBody(connection, bodyStr);
    //    if (ret2 < 0)
    //        return ret2;
    //    request.setBody(bodyStr);
    //}
    std::cout << request;
    connection.isResponseReady = true;
    // parseRequestBody();

    return 0;
}
