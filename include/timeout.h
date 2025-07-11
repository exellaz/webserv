#ifndef TIMEOUT_H
# define TIMEOUT_H

#include <sys/time.h> // gettimeofday, struct timeval

#include "Configuration.h"
#include "Client.h"

time_t getNowInSeconds();
int getNearestUpcomingTimeout(std::vector<Client>& clients, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers);
void disconnectTimedOutClients(std::vector<Client>& clients, std::vector<struct pollfd>& pfds, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers);

#endif