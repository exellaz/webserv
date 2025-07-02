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
#include "Client.h"
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



// Setup Listening Socket
void setupListeningSocket(std::vector<struct pollfd>& pfds, std::vector<int>& listeners, Server& server);

bool isListener(std::vector<int>& listeners, int fd);
Client* findClientByFd(std::vector<Client>& clients, int fd);

void acceptClient(std::vector<struct pollfd>& pfds, std::vector<Client>& clients, int listener);

// handle poll events
void handlePollIn(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
                    struct pollfd& pfd, Client& client);
void handlePollOut(struct pollfd& pfd, Client& client);
void handlePollHup(Client& client);
void handlePollErr(Client& client);


// // Read Request Utils
// int readRequestHeader(Client& client, std::string& headerStr, const size_t bufferSize);
// int readRequestBody(Client& client, std::string& bodyStr, const size_t bufferSize, const size_t maxSize);
// int receiveClientRequest(Client& client, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers);
// int readByChunkedEncoding(Client& client, std::string& bodyStr, const size_t bufferSize, const size_t maxSize);

// Utils
int  setNonBlocking(int fd);
std::vector<Client>::iterator disconnectClient(std::vector<Client>& clients, std::vector<Client>::iterator &clientIt, std::vector<struct pollfd>& pfds);
void clearDisconnectedClients(std::vector<Client>& clients, std::vector<struct pollfd>& pfds);

// utils2
void resolveLocationPath(const std::string& uri, Client& client);
//std::string resolveHttpPath(const std::string& uri, Server &server);
std::string readDirectorytoString(const std::string &directoryPath, const std::string& uri);
std::pair<std::string, std::string> getIpAndPortFromSocketFd(int fd);
Server& getDefaultServerBlockByIpPort(std::pair<std::string, std::string> ipPort, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers);

// PRINT
void printListeners(std::vector<int>& vec);
void printClients(std::vector<Client>& vec);
void printPfds(std::vector<struct pollfd>& vec);

class PollErrorException : public std::exception {
public:
    // 'throw()' specifies that func won't throw any exceptions
    const char* what() const throw() {
        return "Poll error";
    }

};

#endif
