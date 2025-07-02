#ifndef SOCKETS_POLLING_h
#define SOCKETS_POLLING_h


#include <netdb.h> // struct addrinfo, freeaddrinfo (in setupListeningSocket only)
#include <poll.h> // poll(), struct pollfd
#include <iostream>
#include <sstream>
#include <fcntl.h> // setNonBlocking()
#include <vector>
#include <algorithm> // for isListener

#include "Configuration.hpp"
#include "Client.h"
#include "Cgi.hpp"

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h> // memset
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <arpa/inet.h> // inet_top
// #include <dirent.h> //for opendir, readdir, closedir
// #include <sys/stat.h> // for stat
// #include "http-request.h"
// #include "http-response.h"
// #include <cctype>

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define CYAN "\033[36m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define GREY "\033[90m"
#define BLUE "\033[0;34m"

// Setup Listening Socket
void setupListeningSocket(std::vector<struct pollfd>& pfds, std::vector<int>& listeners, Server& server);

// client
void acceptClient(std::vector<struct pollfd>& pfds, std::vector<Client>& clients, int listener);

// Utils
bool isListener(std::vector<int>& listeners, int fd);
int  setNonBlocking(int fd);
Client* findClientByFd(std::vector<Client>& clients, int fd);
std::vector<Client>::iterator disconnectClient(std::vector<Client>& clients, std::vector<Client>::iterator &clientIt, std::vector<struct pollfd>& pfds);
void clearDisconnectedClients(std::vector<Client>& clients, std::vector<struct pollfd>& pfds);

// handle poll events
void handlePollIn(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
                    struct pollfd& pfd, Client& client);
void handlePollOut(struct pollfd& pfd, Client& client);
void handlePollHup(Client& client);
void handlePollErr(Client& client);


// ! Moved functions that are only used once to their individual files
// std::string readDirectorytoString(const std::string &directoryPath, const std::string& uri); // used in handleGetRequest only
// void resolveLocationPath(const std::string& uri, Client& client); // used in handlePollIn() only


// used in recvClientRequest() and timeout functions
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
