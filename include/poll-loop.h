#ifndef POLL_LOOP_H
# define POLL_LOOP_H

#include <poll.h> // poll(), struct pollfd

#include "Configuration.h"
#include "Client.h"
#include "color.h"
#include "handle-sockets.h"

void pollLoop(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
              std::vector<struct pollfd>& pfds,
              std::vector<int>& listeners,
              std::vector<Client>& clients);

void handlePollIn(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
                    struct pollfd& pfd, Client& client);
void handlePollOut(struct pollfd& pfd, Client& client);
void handlePollHup(Client& client);
void handlePollErr(Client& client);

// Poll Loop utils
bool isListener(std::vector<int>& listeners, int fd);
Client& findClientByFd(std::vector<Client>& clients, int fd);
void clearDisconnectedClients(std::vector<Client>& clients, std::vector<struct pollfd>& pfds);

int handleParsingError(const HttpException& e, HttpResponse& response, Client& client);

#endif