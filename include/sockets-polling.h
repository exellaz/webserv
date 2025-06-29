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
#include "Configuration.hpp"
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

#define HEADER_END "\r\n\r\n"
#define CRLF "\r\n"
#define CRLF_LENGTH 2
#define DOUBLE_CRLF_LENGTH 4

enum readReturnVal {
    RECV_OK = 0,
    RECV_AGAIN = -1,
    RECV_CLOSED = -2,
	REQUEST_ERR = -3,
};

// Setup Listening Socket
int setupListeningSocket(std::vector<struct pollfd>& pfds, std::vector<Connection>& connections, Server& server);
// CONNECTIONS
void handlePollIn(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
                    std::vector<struct pollfd>& pfds, std::vector<Connection>& connections,
                    std::vector<int>& listeners, int i);
void handlePollOut(std::vector<struct pollfd>& pfds, std::vector<Connection>& connections, int i);
void handlePollHup(std::vector<Connection>& connections, int i);
void handlePollErr(std::vector<Connection>& connections, int i);
void acceptClient(std::vector<struct pollfd>& pfds, std::vector<Connection>& connections, int listener);

// Read Request Utils
int readRequestHeader(Connection &conn, std::string& headerStr, const size_t bufferSize);
int readRequestBody(Connection &conn, std::string& bodyStr, const size_t bufferSize, const size_t maxSize);
// int receiveClientRequest(Connection &connection, std::map<int, std::vector<Server> >& servers);
int receiveClientRequest(Connection &connection, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers);
int readByChunkedEncoding(Connection &conn, std::string& bodyStr, const size_t bufferSize, const size_t maxSize);

// Timeout
int getNearestUpcomingTimeout(std::vector<Connection>& connections, size_t listenerCount, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers);
void disconnectTimedOutClients(std::vector<Connection>& connections, std::vector<struct pollfd>& pfds, size_t listenerCount, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers);
// Utils
void *getInAddr(struct sockaddr *sa);
int  set_nonblocking(int fd);
void addToPfds(std::vector<struct pollfd>& pfds, int newFd);
//void delFromPfds(std::vector<struct pollfd>& pfds, int i);
void disconnectClient(std::vector<Connection>& connections, std::vector<struct pollfd>& pfds, int index);
time_t getNowInSeconds();
int readFromSocket(Connection &connection, int bufferSize);

// utils2
void resolveLocationPath(const std::string& uri, Connection &connection);
//std::string resolveHttpPath(const std::string& uri, Server &server);
std::string readDirectorytoString(const std::string &directoryPath, const std::string& uri);
std::pair<std::string, std::string> getIpAndPortFromSocketFd(int fd);
Server& getDefaultServerBlockByIpPort(std::pair<std::string, std::string> ipPort, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers);

class PollErrorException : public std::exception {
public:
    // 'throw()' specifies that func won't throw any exceptions
    const char* what() const throw() {
        return "Poll error";
    }

};


#endif
