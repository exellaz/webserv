#include "read-request.h"
#include "poll-loop.h"
#include "utils.h"
#include "session.h"

static void setPfdTrackPollOutOnly(struct pollfd& pfd);
static void setPfdTrackPollInOnly(struct pollfd& pfd);
static void resolveLocationPath(const std::string& uri, Client& client);
static void sendResponseToClient(int fd, HttpResponse& response);

void handlePollIn(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
                    struct pollfd& pfd, Client& client)
{
    std::cout << "POLLIN: socket " << pfd.fd << '\n';

    int res = receiveClientRequest(client, servers);
    if (res == RECV_CLOSED) {
        std::cout << "server: socket " << pfd.fd << " hung up\n";
        client.connState = DISCONNECTED;
        return;
    }
    else if (res == RECV_AGAIN)
        return;
    else if (res != REQUEST_ERR) {
        try {
            resolveLocationPath(client.request.getURI(), client);
            SessionManager::handleSession(client);
            dispatchRequest(client);
        }
        catch (const HttpException& e) {
            handleParsingError(e, client.response, client);
        }
    }
    std::cout << "Response ready\n";
    client.isResponseReady = true;
    client.clearBuffer();
    setPfdTrackPollOutOnly(pfd);
}

void handlePollOut(struct pollfd& pfd, Client& client)
{
    std::cout << "POLLOUT: socket " << client.fd << '\n';

    sendResponseToClient(client.fd, client.response);
    client.isResponseReady = false;
    client.request.clearRequest();
    client.response.clearResponse();

    if (client.connType == CLOSE) {
        client.connState = DISCONNECTED;
        return;
    }
    setPfdTrackPollInOnly(pfd);
}

void handlePollHup(Client& client)
{
    std::cout << "POLLHUP: socket " << client.fd << '\n';
    client.connState = DISCONNECTED;
}

void handlePollErr(Client& client)
{
    std::cout << "POLLERR: socket " << client.fd << '\n';
    client.connState = DISCONNECTED;
}

static void setPfdTrackPollOutOnly(struct pollfd& pfd)
{
    pfd.events &= ~POLLIN;
    pfd.events |= POLLOUT;
}

static void setPfdTrackPollInOnly(struct pollfd& pfd)
{
    pfd.events &= ~POLLOUT;
    pfd.events |= POLLIN;
}

/**
 * @brief normalize the multiple "/" in the relative uri to one "/"
*/
static std::string trimMultipleSlash(const std::string &relativeUri)
{
    std::string result;
    bool lastSlash = false;
    for (size_t i = 0; i < relativeUri.size(); ++i)
    {
        if (relativeUri[i] == '/') {
            if (lastSlash == false)
            {
                result += '/';
                lastSlash = true;
            }
        }
        else
        {
            result += relativeUri[i];
            lastSlash = false;
        }
    }
    return result;
}

/**
 * @brief Validate the relative URI and set the location path
 * @note 1. relativeUri is to identify is there extra value after trim the location path
 * @note 2. check for "/" for prevent multiple slashes in the path to identify as a directory
*/
static void validateRelativeUri(const std::string &relativeUri, Client& client, const std::string &locationType)
{
    std::string getRelativeUri = relativeUri.substr(client.location.getLocaPath().length());
    std::string result = trimMultipleSlash(getRelativeUri);
    if (result.empty() || result == "/")
    {
        if (client.server.getRoot().empty())
            client.locationPath = getFullPath(locationType + result);
        else
            client.locationPath = client.server.getRoot() + locationType + result;
    }
    else
    {
        if (client.server.getRoot().empty())
            client.locationPath = getFullPath(locationType + result);
        else
            client.locationPath = client.server.getRoot() + locationType + result;
    }
}

/**
 * @brief Get the full path from the alias or root
*/
void resolveLocationPath(const std::string& uri, Client& client)
{
    const Location location = client.server.getLocationPath(uri);
    if (!location.getAlias().empty())
        validateRelativeUri(uri, client, location.getAlias());
    else if (!location.getRoot().empty())
        validateRelativeUri(uri, client, location.getRoot());
    else
    {
        if (client.server.getRoot().empty())
            client.locationPath = getFullPath(uri + "/");
        else
            client.locationPath = client.server.getRoot() + uri + "/";
    }
}

static void sendResponseToClient(int fd, HttpResponse& response)
{
    send(fd, response.toString().c_str(), response.toString().size(), 0);
    std::cout << "server: Response sent to client.\n";
}