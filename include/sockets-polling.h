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

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define CYAN "\033[36m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define GREY "\033[90m"
#define BLUE "\033[0;34m"

// Setup Listening Socket
void setupListenerSocket(std::vector<struct pollfd>& pfds, std::vector<int>& listeners, Server& server);
void setupAllListenerSockets(std::map< std::pair<std::string, std::string> , std::vector<Server> > servers,
                             std::vector<struct pollfd>& pfds, 
                             std::vector<int>& listeners);
// socket (listener & client)
int  setNonBlocking(int fd);

// client
void acceptClient(std::vector<struct pollfd>& pfds, std::vector<Client>& clients, int listener);
std::vector<Client>::iterator disconnectClient(std::vector<Client>& clients, std::vector<Client>::iterator &clientIt, std::vector<struct pollfd>& pfds);

// Utils

// used only in poll loop
void handlePollIn(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
                    struct pollfd& pfd, Client& client);
void handlePollOut(struct pollfd& pfd, Client& client);
void handlePollHup(Client& client);
void handlePollErr(Client& client);

void pollLoop(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
              std::vector<struct pollfd>& pfds,
              std::vector<int>& listeners,
              std::vector<Client>& clients);

bool isListener(std::vector<int>& listeners, int fd);
Client& findClientByFd(std::vector<Client>& clients, int fd);
void clearDisconnectedClients(std::vector<Client>& clients, std::vector<struct pollfd>& pfds);

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
