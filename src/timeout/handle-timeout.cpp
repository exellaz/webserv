#include "timeout.h"
#include "client.h"
#include "handle-sockets.h"

time_t getNowInSeconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

static time_t getTimeoutBySocketFd(int fd, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers)
{
    std::pair<std::string, std::string> ipPort = getIpAndPortFromSocketFd(fd);
    Server& defaultServer = getDefaultServerBlockByIpPort(ipPort, servers);

    return defaultServer.getClientTimeout();
}

int getNearestUpcomingTimeout(std::vector<Client>& clients, std::map< std::pair<std::string, std::string>, std::vector<Server> >& servers)
{
    if (clients.empty())
        return -1;
    time_t clientTimeout;

    time_t timeElasped = getNowInSeconds() - clients.begin()->getStartTime();
    clientTimeout = getTimeoutBySocketFd(clients.begin()->getFd(), servers);
    time_t nearestTimeout = clientTimeout - timeElasped;

    std::vector<Client>::iterator clientIt = clients.begin();
    for(; clientIt != clients.end(); ++clientIt) {
        timeElasped = getNowInSeconds() - clientIt->getStartTime();
        clientTimeout = getTimeoutBySocketFd(clientIt->getFd(), servers);
        time_t timeout = clientTimeout - timeElasped;
        if (timeout < nearestTimeout)
            nearestTimeout = timeout;
    }
    return (int)nearestTimeout * 1000; //convert to miliseconds
}

void disconnectTimedOutClients(std::vector<Client>& clients, std::vector<struct pollfd>& pfds,
                                std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers)
{
    if (clients.empty())
        return;

    std::vector<Client>::iterator clientIt = clients.begin();
    while (clientIt != clients.end()) {
        time_t clientTimeout = getTimeoutBySocketFd(clientIt->getFd(), servers);

        if (getNowInSeconds() - clientIt->getStartTime() >= clientTimeout) {
            std::cout << infoTime() << "server: TIMEOUT for client socket " << clientIt->getFd() << '\n';
            //TODO: send response "408 Request Timeout"

            clientIt = disconnectClient(clients, clientIt, pfds);
        }
        else
            ++clientIt;
    }
}


