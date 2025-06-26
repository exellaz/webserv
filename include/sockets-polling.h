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
#include <dirent.h> //for opendir, readdir, closedir
#include <sys/stat.h> // for stat
#include <algorithm> // for find
#include "./Configuration.hpp"
#include "http-request.h"
#include "./Buffer.h"
#include "Connection.h"
#include "http-response.h"
#include <algorithm>
#include <cctype>
#include "Cgi.hpp"


#define RESET "\033[0m"
#define BOLD "\033[1m"
#define CYAN "\033[36m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define GREY "\033[90m"
#define BLUE "\033[0;34m"

#define CLIENT_TIMEOUT 60 // in seconds
#define CLIENT_MAX_BODY_SIZE 1048576

#define HEADER_END "\r\n\r\n"
#define CRLF "\r\n"
#define CRLF_LENGTH 2

enum readReturnVal {
    RECV_OK = 0,
    RECV_AGAIN = -1,
    RECV_CLOSED = -2,
	REQUEST_ERR = -3,
};

// Setup Listening Socket
int setupListeningSocket(std::vector<struct pollfd>& pfds, std::vector<Connection>& connections, Server& server);
// CONNECTIONS
void acceptClient(std::vector<struct pollfd>& pfds, std::vector<Connection>& connections, int listener);

// Read Request Utils
int readRequestHeader(Connection &conn, std::string& headerStr, int bufferSize);
int readRequestBody(Connection &conn, std::string& bodyStr, int bufferSize);
int receiveClientRequest(Connection &connection, std::vector<Server>& servers);
int readByChunkedEncoding(Connection &conn, std::string& bodyStr, int bufferSize);

// Timeout
int getNearestUpcomingTimeout(std::vector<Connection>& connections, size_t listenerCount);
void disconnectTimedOutClients(std::vector<Connection>& connections, std::vector<struct pollfd>& pfds, size_t listenerCount);

// Utils
void *getInAddr(struct sockaddr *sa);
int  set_nonblocking(int fd);
void addToPfds(std::vector<struct pollfd>& pfds, int newFd);
//void delFromPfds(std::vector<struct pollfd>& pfds, int i);
void disconnectClient(std::vector<Connection>& connections, std::vector<struct pollfd>& pfds, int index);
time_t getNowInSeconds();
int readFromSocket(Connection &connection, int bufferSize);

// utils2
//std::string resolveAliasPath(const std::string &url, const Location &location);
//std::string readFileToString (std::ifstream &file);
Server getServerByPort(const std::vector<Server> &servers, const std::string port);
std::string resolveHttpPath(const HttpRequest &request, Server &server);
//bool serveStaticFile(const std::string &httpPath, int clientFd);
//bool serveAutoIndex(const std::string &httpPath, const std::string &url, int clientFd);
std::string readDirectorytoString(const std::string &directoryPath, const HttpRequest &request);
std::string getSocketPortNumber(int fd);

class PollErrorException : public std::exception {
public:
    // 'throw()' specifies that func won't throw any exceptions
    const char* what() const throw() {
        return "Poll error";
    }

};


#endif
