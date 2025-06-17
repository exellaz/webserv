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


    std::string choosePort = request.getHeader("Host").substr(request.getHeader("Host").rfind(':') + 1);
    Config serverConfig = getServerConfigByPort(configs, choosePort);
    Location location = serverConfig.getLocationPath(request.getURI());

    if (!location.cgi_path.empty())
    {
    	std::cout << "CGI found\n"; ////debug
    	Cgi	cgi;
    	std::string res =cgiOutputToHttpResponse(cgi.executeCgi(request)); //? handle by response
    	if (res.empty())
    	{
    		std::string error = "HTTP/1.1 500 Internal Server Error\r\n\r\nCGI execution failed"; //! error
    		send(connection.fd, error.c_str(), error.size(), 0);
    	}
    	else
    	{
    		std::cout << "---------- CGI Output ----------\n" << BLUE << res << RESET << std::endl;
    		send(connection.fd, res.c_str(), res.size(), 0);
    	}
    }


    // parseRequestHeader();
    if (response.getStatus() == OK && request.getMethod() == "GET")
    {
        std::cout << "root: " << configs[0].getLocationPath(request.getURI()).root << "\n";
        //response.handleGetRequest(request, configs[0].getLocationPath(request.getURI()).root);
        //response.handleGetRequest(request, location.root);
        std::string httpPath = resolveHttpPath(request,serverConfig);

        struct stat info;
        if (stat(httpPath.c_str(), &info) == 0 && S_ISREG(info.st_mode) == true)
        {
        	// Method not allowed check
        	if (serveStaticFile(httpPath, connection.fd) == false)
        		return 1;
        }
        else if (location.autoIndex == true && S_ISDIR(info.st_mode) == true)
        {
            if (serveAutoIndex(httpPath, request.getURI(), connection.fd) == false)
                return 1;
        }
    }

    // TODO: isBodyPresent()   -> check Content-Length, Transfer-Encoding, request method

    if (request.getHeaders().find("Content-Length") != request.getHeaders().end()) {
        int ret2 = readRequestBody(connection, bodyStr);
        if (ret2 < 0)
            return ret2;
        request.setBody(bodyStr);
    }
    std::cout << request;
    connection.isResponseReady = true;
    // parseRequestBody();

    //TODO: handle request and response

    //char Buffer[8192];
    //ssize_t byteRead = 0;
    //std::string buf;
    //Config serverConfig;
    //std::string html;
    //std::string httpRes;
    //std::string httpStr;

    ////read the data from the client until the request is complete
    //while (1)
    //{
    //	//return the number of bytes read
    //	byteRead = recv(connection.fd, Buffer, sizeof(Buffer) - 1, 0);
    //	//read the data from the client and append to the buffer
    //	buf.append(Buffer, byteRead);
    //	//check if the request is complete
    //	if (request.parseRequestBody(buf) == true
    //		&& request.parseHeaderLines(buf, response) == true
    //		&& request.parseRequestLine(buf, response) == true)
    //		break;
    //}

    //// if nBytes is less than or equal to 0, the client has closed the connection or an error occurred
    //if (byteRead <= 0)
    //{
    //	if (byteRead == 0)
    //		std::cout << "pollserver: socket " << connection.fd << " hung up\n";
    //	else
    //		perror("recv");
    //	close(connection.fd);
    //	return 1;
    //}

    //std::cout << "-----------------------------\n";
    //std::cout << "Data from Client: \n"<< buf << "\n-----------------------------\n";

    //std::string choosePort = request.getHeader("Host").substr(request.getHeader("Host").rfind(':') + 1);
    //serverConfig = getServerConfigByPort(configs, choosePort);

    //Location location = serverConfig.getLocationPath(request.getURI());

    //if (!location.cgi_path.empty())
    //{
    //	std::cout << "CGI found\n"; ////debug
    //	Cgi	cgi;
    //	std::string res =cgiOutputToHttpResponse(cgi.executeCgi(request)); //? handle by response
    //	if (res.empty())
    //	{
    //		std::string error = "HTTP/1.1 500 Internal Server Error\r\n\r\nCGI execution failed"; //! error
    //		send(connection.fd, error.c_str(), error.size(), 0);
    //	}
    //	else
    //	{
    //		std::cout << "---------- CGI Output ----------\n" << BLUE << res << RESET << std::endl;
    //		send(connection.fd, res.c_str(), res.size(), 0);
    //	}
    //}

    //std::string httpPath = resolveHttpPath(request,serverConfig);
    //if (httpPath.empty())
    //{
    //	httpRes = "HTTP/1.1 404 NotFound\r\n" //!error
    //		"Content-Type: text/html\r\n"
    //		"\r\n"
    //		"404 Not Found";
    //	send(connection.fd, httpRes.c_str(), httpRes.size(), 0);
    //	return 1;
    //}

    //struct stat info;
    //if (stat(httpPath.c_str(), &info) == 0 && S_ISREG(info.st_mode) == true)
    //{
    //	// Method not allowed check
    //	if (!serverConfig.getAllowMethods().empty() && request.getMethod() != "GET") //? handle by response
    //	{
    //		std::string errorstr = "Method Not Allowed";
    //		std::ostringstream errorLen;
    //		errorLen << errorstr.size();
    //		std::string error = "HTTP/1.1 405 Method Not Allowed\r\n"
    //							"Content-Type: text/html\r\n"
    //							"Content-Length: " + errorLen.str() + "\r\n"
    //							"\r\n"
    //							+ errorstr;
    //		send(connection.fd, error.c_str(), error.size(), 0);
    //		return 1;
    //	}
    //	if (serveStaticFile(httpPath, connection.fd) == false)
    //		return 1;
    //}
    //else if (location.autoIndex == true && S_ISDIR(info.st_mode) == true)
    //{
    //	if (!serverConfig.getAllowMethods().empty() && request.getMethod() == "DELETE") //? handle by response
    //	{
    //		std::string errorstr = "Method Not Allowed: DELETE method is not allowed for directories";
    //		std::ostringstream errorLen;
    //		errorLen << errorstr.size();
    //		std::string error = "HTTP/1.1 405 Method Not Allowed\r\n"
    //							"Content-Type: text/html\r\n"
    //							"Content-Length: " + errorLen.str() + "\r\n"
    //							"\r\n"
    //							+ errorstr;
    //		send(connection.fd, error.c_str(), error.size(), 0);
    //		return 1;
    //	}
    //	else if (!serverConfig.getAllowMethods().empty() && request.getMethod() == "POST") //? handle by response
    //	{
    //		std::string errorstr = "Forbidden: POST method is forbidden for directories";
    //		std::ostringstream errorLen;
    //		errorLen << errorstr.size();
    //		std::string error = "HTTP/1.1 403 Forbidden\r\n"
    //							"Content-Type: text/html\r\n"
    //							"Content-Length: " + errorLen.str() + "\r\n"
    //							"\r\n"
    //							+ errorstr;
    //		send(connection.fd, error.c_str(), error.size(), 0);
    //		return 1;
    //	}
    //	if (serveAutoIndex(httpPath, request.getURI(), connection.fd) == false)
    //		return 1;
    //}

    //httpRes = "HTTP/1.1 404 NotFound\r\n" //!error
    //	"Content-Type: text/html\r\n"
    //	"\r\n"
    //	"404 Not Found";
    //send(connection.fd, httpRes.c_str(), httpRes.size(), 0);

    return 0;
}
