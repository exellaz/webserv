#include "http-request.h"
#include "http-response.h"
#include "client.h"
#include "handle-sockets.h"
#include "session.h"
#include "utils.h"

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

    // Initialise `readBodyMethod`
    if (request.hasHeader("Content-Length"))
        _readBodyMethod = CONTENT_LENGTH;
    else if (request.hasHeader("Transfer-Encoding"))
        _readBodyMethod = CHUNKED_ENCODING;
    else
        _readBodyMethod = NO_BODY;

    if (_readBodyMethod != NO_BODY) {
        std::string bodyStr;

        int ret2 = readRequestBody(bodyStr, defaultServer.getClientBodyBufferSize(), defaultServer.getClientMaxSize());
        if (ret2 < 0)
            return ret2;
        request.setBody(bodyStr);
    }

    std::cout << infoTime() << "Request received from client.\n";
    return RECV_OK;
}
