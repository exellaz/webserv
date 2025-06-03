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
#include <fcntl.h>
#include <vector>
#include "./Configuration.hpp"
#include "./Buffer.h"

#define PORT "8080"
#define HEADER_BUFFER_SIZE 1024
#define BODY_BUFFER_SIZE 8192

#define NGX_AGAIN -1
#define NGX_OK 0
#define NGX_ERROR -2
#define NGX_REQUEST_HEADER_TOO_LARGE -431
#define CLIENT_HEADER_BUFFER_SIZE 4096
#define LARGE_HEADER_BUFFER_SIZE 8192
#define MAX_LARGE_BUFFERS 4


// Setup Listening Socket
int setupListeningSocket(std::vector<struct pollfd>& pfds, Config& config);

// Connections
void acceptClient(std::vector<struct pollfd>& pfds, int listener);
void readRequestHeader(int fd, std::string& headerStr, std::vector<Buffer>& buffers);
int receiveClientRequest(int fd);

// Utils
void *getInAddr(struct sockaddr *sa);
int  set_nonblocking(int fd);
void addToPfds(std::vector<struct pollfd>& pfds, int newFd);
void delFromPfds(std::vector<struct pollfd>& pfds, int i);


#endif
