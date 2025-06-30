#include "../../include/sockets-polling.h"


static time_t getTimeoutBySocketFd(int fd, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers)
{
	std::pair<std::string, std::string> ipPort = getIpAndPortFromSocketFd(fd);
    Server& defaultServer = getDefaultServerBlockByIpPort(ipPort, servers);

	return defaultServer.getClientTimeout();
}

int getNearestUpcomingTimeout(std::vector<Client>& clients, size_t listenerCount,
								std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers)
{
	if (clients.size() == listenerCount) // only listener fds
		return -1;
	time_t clientTimeout;

	time_t timeElasped = getNowInSeconds() - clients[listenerCount].startTime;
	clientTimeout = getTimeoutBySocketFd(clients[listenerCount].fd, servers);
	time_t nearestTimeout = clientTimeout - timeElasped;

	for (size_t i = listenerCount; i < clients.size(); ++i) {
		timeElasped = getNowInSeconds() - clients[i].startTime;
		clientTimeout = getTimeoutBySocketFd(clients[listenerCount].fd, servers);
		time_t timeout = clientTimeout - timeElasped;
		if (timeout < nearestTimeout)
			nearestTimeout = timeout;
	}
	return (int)nearestTimeout * 1000; //convert to miliseconds
}

void disconnectTimedOutClients(std::vector<Client>& clients, std::vector<struct pollfd>& pfds, size_t listenerCount,
								std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers)
{
	if (clients.size() == listenerCount) // only listener fds
		return;

	for (size_t i = listenerCount; i < clients.size(); ++i) { //? starting index base on the number of listeners
		time_t clientTimeout = getTimeoutBySocketFd(clients[i].fd, servers);

		if (getNowInSeconds() - clients[i].startTime >= clientTimeout) {
			std::cout << "server: TIMEOUT for client socket " << clients[i].fd << '\n';
			//TODO: send response "408 Request Timeout"

			disconnectClient(clients, pfds, i);
			i--;
			continue;
		}
	}

}


