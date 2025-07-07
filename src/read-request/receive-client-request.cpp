#include "http-request.h"
#include "read-request.h"
#include "handle-sockets.h"
#include "session.h"

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

static void validateMethod(const std::string& method, const std::vector<std::string>& allowedMethods)
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
