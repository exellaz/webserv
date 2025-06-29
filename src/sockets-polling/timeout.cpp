#include "../../include/sockets-polling.h"


static time_t getTimeoutBySocketFd(int fd, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers)
{
	std::pair<std::string, std::string> ipPort = getIpAndPortFromSocketFd(fd);
    Server& defaultServer = getDefaultServerBlockByIpPort(ipPort, servers);

	return defaultServer.getClientTimeout();
}

int getNearestUpcomingTimeout(std::vector<Connection>& connections, size_t listenerCount,
								std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers)
{
	if (connections.size() == listenerCount) // only listener fds
		return -1;
	time_t clientTimeout;

	time_t timeElasped = getNowInSeconds() - connections[listenerCount].startTime;
	clientTimeout = getTimeoutBySocketFd(connections[listenerCount].fd, servers);
	time_t nearestTimeout = clientTimeout - timeElasped;

	for (size_t i = listenerCount; i < connections.size(); ++i) {
		timeElasped = getNowInSeconds() - connections[i].startTime;
		clientTimeout = getTimeoutBySocketFd(connections[listenerCount].fd, servers);
		time_t timeout = clientTimeout - timeElasped;
		if (timeout < nearestTimeout)
			nearestTimeout = timeout;
	}
	return (int)nearestTimeout * 1000; //convert to miliseconds
}

void disconnectTimedOutClients(std::vector<Connection>& connections, std::vector<struct pollfd>& pfds, size_t listenerCount,
								std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers)
{
	if (connections.size() == listenerCount) // only listener fds
		return;

	for (size_t i = listenerCount; i < connections.size(); ++i) { //? starting index base on the number of listeners
		time_t clientTimeout = getTimeoutBySocketFd(connections[i].fd, servers);

		if (getNowInSeconds() - connections[i].startTime >= clientTimeout) {
			std::cout << "server: TIMEOUT for client socket " << connections[i].fd << '\n';
			//TODO: send response "408 Request Timeout"

			disconnectClient(connections, pfds, i);
			i--;
			continue;
		}
	}

}


