#include "Client.h"
#include "poll-loop.h"
#include "Cgi.hpp"
#include "session.h"

static void setPfdTrackPollOutOnly(struct pollfd& pfd);
static void setPfdTrackPollInOnly(struct pollfd& pfd);
static void resolveLocationPath(const std::string& uri, Client& client);

void handlePollIn(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
                    struct pollfd& pfd, Client& client)
{
    std::cout << "POLLIN: socket " << pfd.fd << '\n';

    int res = client.receiveClientRequest(servers);
    if (res == RECV_CLOSED) {
        std::cout << "server: socket " << pfd.fd << " hung up\n";
        client.setConnState(DISCONNECTED);
        return;
    }
    else if (res == RECV_AGAIN)
        return;
    else if (res != REQUEST_ERR) {
        try {
            resolveLocationPath(client.request.getURI(), client);
            SessionManager::handleSession(client);
            client.dispatchRequest();
        }
        catch (const HttpException& e) {
            handleParsingError(e, client.response, client);
        }
    }
    std::cout << "Response ready\n";
    client.setResponseReady(true);
    client.clearBuffer();
    setPfdTrackPollOutOnly(pfd);
}

void handlePollOut(struct pollfd& pfd, Client& client)
{
    std::cout << "POLLOUT: socket " << client.getFd() << '\n';

    client.sendResponseToClient();
    client.setResponseReady(false);
    client.request.clearRequest();
    client.response.clearResponse();

    if (client.getConnType() == CLOSE) {
        client.setConnState(DISCONNECTED);
        return;
    }
    setPfdTrackPollInOnly(pfd);
}

void handlePollHup(Client& client)
{
    std::cout << "POLLHUP: socket " << client.getFd() << '\n';
    client.setConnState(DISCONNECTED);
}

void handlePollErr(Client& client)
{
    std::cout << "POLLERR: socket " << client.getFd() << '\n';
    client.setConnState(DISCONNECTED);
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
    std::cout << "Relative path: " << result << "\n"; ////debug
    if (result.empty() || result == "/")
    {
        std::cout << GREEN "Alias/Root path: " << getFullPath(locationType + result) << "\n" RESET; ////debug
        if (client.server.getRoot().empty())
            client.setLocationPath(getFullPath(locationType + result));
        else
            client.setLocationPath(client.server.getRoot() + locationType + result);
    }
    else
    {
        std::cout << GREEN "Alias/Root path extra value: " << getFullPath(locationType + result) << "\n" RESET; ////debug
        if (client.server.getRoot().empty())
            client.setLocationPath(getFullPath(locationType + result));
        else
            client.setLocationPath(client.server.getRoot() + locationType + result);
    }
}

/**
 * @brief Get the full path from the alias or root
*/
void resolveLocationPath(const std::string& uri, Client& client)
{
    const Location location = client.server.getLocationPath(uri);
    if (!location.getAlias().empty())
    {
        std::cout << "Alias found\n"; ////debug"
        validateRelativeUri(uri, client, location.getAlias());
    }
    else if (!location.getRoot().empty())
    {
        std::cout << "Root found\n"; ////debug
        validateRelativeUri(uri, client, location.getRoot());
    }
    else
    {
        std::cout << RED "No alias or root found for the location path: " << uri << "\n" RESET; ////debug
        if (client.server.getRoot().empty())
            client.setLocationPath(getFullPath(uri + "/"));
        else
            client.setLocationPath(client.server.getRoot() + uri + "/");
    }
}

