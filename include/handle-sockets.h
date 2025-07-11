#ifndef HANDLE_SOCKETS_H
# define HANDLE_SOCKETS_H

#include <poll.h> // poll(), struct pollfd
#include <netdb.h> // struct addrinfo, freeaddrinfo
#include <unistd.h> // close

#include "configuration.h"
#include "client.h"

// listener
void setupListenerSocket(std::vector<struct pollfd>& pfds, std::vector<int>& listeners, Server& server);
void setupAllListenerSockets(std::map< std::pair<std::string, std::string> , std::vector<Server> > servers,
                             std::vector<struct pollfd>& pfds,
                             std::vector<int>& listeners);

// client
void acceptClient(std::vector<struct pollfd>& pfds, std::vector<Client>& clients, int listener);
std::vector<Client>::iterator disconnectClient(std::vector<Client>& clients, std::vector<Client>::iterator &clientIt, std::vector<struct pollfd>& pfds);

// utils
int  setNonBlocking(int fd);
std::pair<std::string, std::string> getIpAndPortFromSocketFd(int fd);
void closeAllSockets(std::vector<struct pollfd>& pfds);

#endif
