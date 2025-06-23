#include "../../include/sockets-polling.h"

int getNearestUpcomingTimeout(std::vector<Connection>& connections, size_t listenerCount)
{
	if (connections.size() == listenerCount) // only listener fds 	
		return -1;
	time_t timeElasped = getNowInSeconds() - connections[listenerCount].startTime;
	time_t nearestTimeout = CLIENT_TIMEOUT - timeElasped;

	for (size_t i = listenerCount; i < connections.size(); ++i) {
		timeElasped = getNowInSeconds() - connections[i].startTime;
		time_t timeout = CLIENT_TIMEOUT - timeElasped;
		if (timeout < nearestTimeout)
			nearestTimeout = timeout;
	}
	return (int)nearestTimeout * 1000; //convert to miliseconds
}

void disconnectTimedOutClients(std::vector<Connection>& connections, std::vector<struct pollfd>& pfds, size_t listenerCount)
{
	if (connections.size() == listenerCount) { // only listener fds
		return;
	}

	for (size_t i = listenerCount; i < connections.size(); ++i) { //? starting index base on the number of listeners

		if (getNowInSeconds() - connections[i].startTime >= CLIENT_TIMEOUT) {
			std::cout << "server: TIMEOUT for client socket " << connections[i].fd << '\n';
			//TODO: send response "408 Request Timeout"

			disconnectClient(connections, pfds, i);
			i--;
			continue;
		}
	}

}


