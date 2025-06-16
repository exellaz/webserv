#include "../../include/sockets-polling.h"

int getNearestUpcomingTimeout(std::vector<Connection>& connections)
{
	if (connections.size() == 1) { // only listener fd
		return -1;
	}
	time_t timeElasped = getNowInSeconds() - connections[1].startTime;
	time_t nearestTimeout = CLIENT_HEADER_TIMEOUT - timeElasped;

	for (size_t i = 2; i < connections.size(); ++i) {
		timeElasped = getNowInSeconds() - connections[i].startTime;
		time_t timeout = CLIENT_HEADER_TIMEOUT - timeElasped;
		if (timeout < nearestTimeout)
			nearestTimeout = timeout;
	}
	return (int)nearestTimeout * 1000; //convert to miliseconds
}

void disconnectTimedOutClients(std::vector<Connection>& connections, std::vector<struct pollfd>& pfds)
{
	if (connections.size() == 1) { // only listener fd
		return;
	}

	for (size_t i = 1; i < connections.size(); ++i) {
		
		// TODO: need to check status of connection? (READING_HEADER, READING_BODY...)
		if (getNowInSeconds() - connections[i].startTime >= CLIENT_HEADER_TIMEOUT) {
			std::cout << "server: TIMEOUT for client socket " << connections[i].fd << '\n';
			//TODO: send response "408 Request Timeout"

			disconnectClient(connections, pfds, i);
			i--;
			continue;
		}
	}

}


