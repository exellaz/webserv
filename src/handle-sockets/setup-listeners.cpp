#include <string.h> // memset
#include "handle-sockets.h"

// Return a listening socket
static int getListenerSocket(const std::string& host, const std::string& port)
{
    int listener;     // Listening socket descriptor
    int rv;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *p;


    // Get socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(host.c_str(), port.c_str(), &hints, &res)) != 0)
        throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(rv));

    for(p = res; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
            continue;

        int yes = 1;
        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        break;
    }

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);

    // Listen
    if (listen(listener, 10) == -1)
        return -1;

    return listener;
}

// returns listening socket fd
void setupListenerSocket(std::vector<struct pollfd>& pfds, std::vector<int>& listeners, Server& server) {

    int listenerFd = getListenerSocket(server.getHost(), server.getPort());
    if (listenerFd == -1)
        throw std::runtime_error("error getting listening socket");

    if (setNonBlocking(listenerFd) == -1) {
        perror("setNonBlocking");
        close(listenerFd);
        throw std::runtime_error("error setting fd to non-blocking");
    }

    // Add the listenerFd to set
    struct pollfd pfd;
    pfd.fd = listenerFd;
    pfd.events = POLLIN; // Report ready to read on incoming connection
    pfds.push_back(pfd);

    listeners.push_back(listenerFd);

    std::cout << infoTime() << "Listener socket fd: " << listenerFd << " binded to " << server.getHost() << ":" << server.getPort() << "\n";
}


void setupAllListenerSockets(std::map< std::pair<std::string, std::string> , std::vector<Server> > servers,
                             std::vector<struct pollfd>& pfds,
                             std::vector<int>& listeners)
{
    std::map<std::pair<std::string, std::string>, std::vector<Server> >::iterator it = servers.begin();
    for (; it != servers.end(); ++it)
    {
        Server &curServer = *(it->second.begin());
        setupListenerSocket(pfds, listeners, curServer);
    }
}

