#include "../../include/sockets-polling.h"


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

    time_t timeElasped = getNowInSeconds() - clients.begin()->startTime;
    clientTimeout = getTimeoutBySocketFd(clients.begin()->fd, servers);
    time_t nearestTimeout = clientTimeout - timeElasped;

    std::vector<Client>::iterator clientIt = clients.begin();
    for(; clientIt != clients.end(); ++clientIt) {
        timeElasped = getNowInSeconds() - clientIt->startTime;
        clientTimeout = getTimeoutBySocketFd(clientIt->fd, servers);
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

    // for (size_t i = 0; i < clients.size(); ++i) {
    std::vector<Client>::iterator clientIt = clients.begin();
    for(; clientIt != clients.end();) {
        time_t clientTimeout = getTimeoutBySocketFd(clientIt->fd, servers);

        if (getNowInSeconds() - clientIt->startTime >= clientTimeout) {
            std::cout << "server: TIMEOUT for client socket " << clientIt->fd << '\n';
            //TODO: send response "408 Request Timeout"

            disconnectClient(clients, clientIt, pfds);
            continue;
        }
        else 
            ++clientIt;
    }
}


