#include "../include/sockets-polling.h"

// return true
void handlePollIn(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
                    std::vector<struct pollfd>& pfds, std::vector<Connection>& connections,
                    std::vector<int>& listeners, int i)
{
    std::cout << "POLLIN: socket " << connections[i].fd << '\n';

    if (find(listeners.begin(), listeners.end(), pfds[i].fd) != listeners.end())
        acceptClient(pfds, connections, pfds[i].fd);
    else { // if not a listener socket, then it's a client socket
        int res = receiveClientRequest(connections[i], servers);
        if (res == RECV_CLOSED) {
            std::cout << "server: socket " << pfds[i].fd << " hung up\n";
            connections[i].connState = DISCONNECTED;
            return;
        }
        else if (res == RECV_AGAIN)
            return;
        else if (res != REQUEST_ERR) {
            try {
                resolveLocationPath(connections[i].request.getURI(), connections[i]);
                std::cout << "Only path: " << connections[i].locationPath << "\n"; ////debug
                dispatchRequest(connections[i]);
            }
            catch (const HttpException& e) {
                handleParsingError(e, connections[i].response, connections[i]);
            }
        }
        std::cout << "Res: " << res << "\n";
        std::cout << "Response ready\n";
        connections[i].isResponseReady = true;
        connections[i].clearBuffer();
        pfds[i].events &= ~POLLIN;
        pfds[i].events |= POLLOUT;
    }
}

static void sendResponseToClient(int fd, HttpResponse& response)
{
    send(fd, response.toString().c_str(), response.toString().size(), 0);
    std::cout << "server: Response sent to client.\n";
}

void handlePollOut(std::vector<struct pollfd>& pfds, std::vector<Connection>& connections, int i)
{
    std::cout << "POLLOUT: socket " << connections[i].fd << '\n';

    sendResponseToClient(connections[i].fd, connections[i].response);
    connections[i].isResponseReady = false;
    connections[i].request.clearRequest();
    connections[i].response.clearResponse();

    if (connections[i].connType == CLOSE) {
        connections[i].connState = DISCONNECTED;
        return;
    }
    pfds[i].events &= ~POLLOUT;
    pfds[i].events |= POLLIN;
}

void handlePollHup(std::vector<Connection>& connections, int i)
{
    std::cout << "POLLHUP: socket " << connections[i].fd << '\n';
    connections[i].connState = DISCONNECTED;
}

void handlePollErr(std::vector<Connection>& connections, int i)
{
    std::cout << "POLLERR: socket " << connections[i].fd << '\n';
    connections[i].connState = DISCONNECTED;
}
