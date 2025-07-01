#include "../include/sockets-polling.h"
#include "../include/read-request.h"

static void setPfdTrackPollOutOnly(struct pollfd& pfd) {
    pfd.events &= ~POLLIN;
    pfd.events |= POLLOUT;
}

static void setPfdTrackPollInOnly(struct pollfd& pfd) {
    pfd.events &= ~POLLOUT;
    pfd.events |= POLLIN;
}

void handlePollIn(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
                    struct pollfd& pfd, Client& client)
{
    std::cout << "POLLIN: socket " << pfd.fd << '\n';

    int res = receiveClientRequest(client, servers);
    if (res == RECV_CLOSED) {
        std::cout << "server: socket " << pfd.fd << " hung up\n";
        client.connState = DISCONNECTED;
        return;
    }
    else if (res == RECV_AGAIN)
        return;
    else if (res != REQUEST_ERR) {
        try {
            resolveLocationPath(client.request.getURI(), client);
            dispatchRequest(client);
        }
        catch (const HttpException& e) {
            handleParsingError(e, client.response, client);
        }
    }
    std::cout << "Response ready\n";
    client.isResponseReady = true;
    client.clearBuffer();
    setPfdTrackPollOutOnly(pfd);
}

static void sendResponseToClient(int fd, HttpResponse& response)
{
    send(fd, response.toString().c_str(), response.toString().size(), 0);
    std::cout << "server: Response sent to client.\n";
}

void handlePollOut(struct pollfd& pfd, Client& client)
{
    std::cout << "POLLOUT: socket " << client.fd << '\n';

    sendResponseToClient(client.fd, client.response);
    client.isResponseReady = false;
    client.request.clearRequest();
    client.response.clearResponse();

    if (client.connType == CLOSE) {
        client.connState = DISCONNECTED;
        return;
    }
    setPfdTrackPollInOnly(pfd);
}

void handlePollHup(Client& client)
{
    std::cout << "POLLHUP: socket " << client.fd << '\n';
    client.connState = DISCONNECTED;
}

void handlePollErr(Client& client)
{
    std::cout << "POLLERR: socket " << client.fd << '\n';
    client.connState = DISCONNECTED;
}
