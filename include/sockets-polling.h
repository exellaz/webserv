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
#include "Connection.h"
#include "http-response.h"
#include <algorithm>
#include <cctype>

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define CYAN "\033[36m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define GREY "\033[90m"

#define PORT "8080"

#define NGX_AGAIN -1
#define NGX_OK 0
#define NGX_ERROR -2
#define NGX_REQUEST_HEADER_TOO_LARGE -431
#define HEADER_BUFFER_SIZE 1024 // defines the size of the buffer allocated
#define BODY_BUFFER_SIZE 8192
#define CLIENT_TIMEOUT 60 // in seconds
#define MAX_BODY_SIZE 1048576
#define HEADER_END "\r\n\r\n"
#define CRLF "\r\n"
#define CRLF_LENGTH 2

enum recvResult {
    RECV_OK = 0,
    RECV_AGAIN = -1,
    RECV_CLOSED = -2,
};

// Setup Listening Socket
int setupListeningSocket(std::vector<struct pollfd>& pfds, std::vector<Connection>& connections, Config& config);
// CONNECTIONS
void acceptClient(std::vector<struct pollfd>& pfds, std::vector<Connection>& connections, int listener);

// Read Request Utils
int readRequestHeader(Connection &connection, std::string& headerStr);
int recvBodyFromSocket(Connection &connection);
int readByChunkedEncoding(Connection &conn, std::string& bodyStr);
int readRequestBody(Connection &conn, std::string& bodyStr);
int receiveClientRequest(Connection &connection);

// Timeout
void disconnectTimedOutClients(std::vector<Connection>& connections, std::vector<struct pollfd>& pfds);

// Utils
void *getInAddr(struct sockaddr *sa);
int  set_nonblocking(int fd);
void addToPfds(std::vector<struct pollfd>& pfds, int newFd);
void delFromPfds(std::vector<struct pollfd>& pfds, int i);
void disconnectClient(std::vector<Connection>& connections, std::vector<struct pollfd>& pfds, int index);
time_t getNowInSeconds();
int getNearestUpcomingTimeout(std::vector<Connection>& connections);

class BadRequestException : public std::exception {
public:
	// 'throw()' specifies that func won't throw any exceptions
	const char* what() const throw() {
		return "400: Bad Request";
	}

};

class PollErrorException : public std::exception {
public:
	// 'throw()' specifies that func won't throw any exceptions
	const char* what() const throw() {
		return "Poll error";
	}

};


#endif
