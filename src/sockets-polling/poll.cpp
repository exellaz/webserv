#include "../include/sockets-polling.h"

// return true
void handlePollIn(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
                    std::vector<struct pollfd>& pfds, std::vector<Client>& clients,
                    std::vector<int>& listeners, int i)
{
    std::cout << "POLLIN: socket " << clients[i].fd << '\n';

    if (find(listeners.begin(), listeners.end(), pfds[i].fd) != listeners.end())
        acceptClient(pfds, clients, pfds[i].fd);
    else { // if not a listener socket, then it's a client socket
        int res = receiveClientRequest(clients[i], servers);
        if (res == RECV_CLOSED) {
            std::cout << "server: socket " << pfds[i].fd << " hung up\n";
            clients[i].connState = DISCONNECTED;
            return;
        }
        else if (res == RECV_AGAIN)
            return;
        else if (res != REQUEST_ERR) {
            try {
                dispatchRequest(clients[i]);
            }
            catch (const HttpException& e) {
                handleParsingError(e, clients[i].response, clients[i]);
            }
        }
        std::cout << "Res: " << res << "\n";
        std::cout << "Response ready\n";
        clients[i].isResponseReady = true;
        clients[i].clearBuffer();
        pfds[i].events &= ~POLLIN;
        pfds[i].events |= POLLOUT;
    }
}

static void sendResponseToClient(int fd, HttpResponse& response)
{
    send(fd, response.toString().c_str(), response.toString().size(), 0);
    std::cout << "server: Response sent to client.\n";
}

void handlePollOut(std::vector<struct pollfd>& pfds, std::vector<Client>& clients, int i)
{
    std::cout << "POLLOUT: socket " << clients[i].fd << '\n';

    sendResponseToClient(clients[i].fd, clients[i].response);
    clients[i].isResponseReady = false;
    clients[i].request.clearRequest();
    clients[i].response.clearResponse();

    if (clients[i].connType == CLOSE) {
        clients[i].connState = DISCONNECTED;
        return;
    }
    pfds[i].events &= ~POLLOUT;
    pfds[i].events |= POLLIN;
}

void handlePollHup(std::vector<Client>& clients, int i)
{
    std::cout << "POLLHUP: socket " << clients[i].fd << '\n';
    clients[i].connState = DISCONNECTED;
}

void handlePollErr(std::vector<Client>& clients, int i)
{
    std::cout << "POLLERR: socket " << clients[i].fd << '\n';
    clients[i].connState = DISCONNECTED;
}
