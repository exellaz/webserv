#include "http-request.h"
#include "read-request.h"
#include "handle-sockets.h"
#include "session.h"

int handleParsingError(const HttpException& e, HttpResponse& response, Client& client)
{
    std::cerr << RED <<"HTTP Error: " << e.getStatusCode() << " - " << e.what() << "\n";
    response.setStatus(e.getStatusCode());
    // Set body here being the respective error page
    response.setHeader("Connection", "close");
    client.setConnType(CLOSE);
    client.setResponseReady(true);
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
int Client::receiveClientRequest(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers)
{
    // IP:PORT pair from fd
    std::pair<std::string, std::string> ipPort = getIpAndPortFromSocketFd(_fd);

    // get default block by IP:PORT pair
    Server& defaultServer = getDefaultServerBlockByIpPort(ipPort, servers);

    if (!request.isHeaderParsed()) {
        try {
            std::string headerStr;
            int ret = readRequestHeader(headerStr, defaultServer.getClientHeaderBufferSize());
            if (ret < 0)
                return ret;
            request.parseRequestLine(headerStr);
            request.parseHeaderLines(headerStr);
            assignServerByServerName(servers, ipPort, defaultServer);
            location = server.getLocationPath(request.getURI());
            validateMethod(request.getMethod(), location.getAllowMethods());
        }
        catch (const HttpException& e) {
            return handleParsingError(e, response, *this);
        }
    }

    // Initialise `readBodyMethod`
    if (request.hasHeader("Content-Length"))
        _readBodyMethod = CONTENT_LENGTH;
    else if (request.hasHeader("Transfer-Encoding"))
        _readBodyMethod = CHUNKED_ENCODING;
    else
        _readBodyMethod = NO_BODY;

    if (_readBodyMethod != NO_BODY) {
        try {
            std::string bodyStr;

            int ret2 = readRequestBody(bodyStr, defaultServer.getClientBodyBufferSize(), defaultServer.getClientMaxSize());
            if (ret2 < 0)
                return ret2;
            request.setBody(bodyStr);
        }
        catch (const HttpException& e) {
            return handleParsingError(e, response, *this);
        }
    }
    return RECV_OK;
}
