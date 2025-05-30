#ifndef SOCKETS_POLLING_h
#define SOCKETS_POLLING_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <vector>
#include <iostream>
#include <sstream>
#include "./Configuration.hpp"

#define PORT "8080"   // Port we're listening on

class Config;

// Setup Listening Socket
int setupListeningSocket(std::vector<struct pollfd>& pfds, Config& config);

// Connections
void acceptClient(std::vector<struct pollfd>& pfds, int listener);
void receiveClientData(struct pollfd& pfd);

// Utils
void *getInAddr(struct sockaddr *sa);
void addToPfds(std::vector<struct pollfd>& pfds, int newFd);
void delFromPfds(std::vector<struct pollfd>& pfds, int i);


#endif
