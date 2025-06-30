#include "../include/sockets-polling.h"

// void handlePollIn(std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers,
//                     std::vector<struct pollfd>& pfds, std::vector<Client>& clients,
//                     std::vector<int>& listeners, int i)
// {
//     std::cout << "POLLIN: socket " << clients[i].fd << '\n';

//     if (find(listeners.begin(), listeners.end(), pfds[i].fd) != listeners.end())
//         acceptClient(pfds, clients, pfds[i].fd);
//     else { // if not a listener socket, then it's a client socket
//         int res = receiveClientRequest(clients[i], servers);
//         if (res == RECV_CLOSED) {
//             std::cout << "server: socket " << pfds[i].fd << " hung up\n";
//             clients[i].connState = DISCONNECTED;
//             return;
//         }
//         else if (res == RECV_AGAIN)
//             return;
//         else if (res != REQUEST_ERR) {
//             try {
//                 dispatchRequest(clients[i]);
//             }
//             catch (const HttpException& e) {
//                 handleParsingError(e, clients[i].response, clients[i]);
//             }
//         }
//         std::cout << "Res: " << res << "\n";
//         std::cout << "Response ready\n";
//         clients[i].isResponseReady = true;
//         clients[i].clearBuffer();
//         pfds[i].events &= ~POLLIN;
//         pfds[i].events |= POLLOUT;
//     }
// }





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
            dispatchRequest(client);
        }
        catch (const HttpException& e) {
            handleParsingError(e, client.response, client);
        }
    }
    std::cout << "Res: " << res << "\n";
    std::cout << "Response ready\n";
    client.isResponseReady = true;
    client.clearBuffer();
    pfd.events &= ~POLLIN;
    pfd.events |= POLLOUT;
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
    pfd.events &= ~POLLOUT;
    pfd.events |= POLLIN;
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
