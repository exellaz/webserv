#include "sockets-polling.h"

// Get sockaddr, IPv4 or IPv6:
void *getInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Return a listening socket
int getListenerSocket(void)
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

    if ((rv = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    
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
    if (p == NULL)
        return -1;

    freeaddrinfo(res); 

    // Listen
    if (listen(listener, 10) == -1)
        return -1;

    return listener;
}

// Add a new file descriptor to the set
void addToPfds(std::vector<struct pollfd>& pfds, int newFd)
{
	struct pollfd pfd;

	pfd.fd = newFd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	pfds.push_back(pfd);
}

// Remove an index from the set
void delFromPfds(std::vector<struct pollfd>& pfds, int i)
{
	pfds.erase(pfds.begin() + i);
}

// Main
int main(void)
{
    int listener;     // Listening socket descriptor
    int newFd;        // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteAddr; // Client address
    socklen_t addrLen;
    char buf[256];    // Buffer for client data
    char remoteIp[INET6_ADDRSTRLEN];

	std::vector<struct pollfd> pfds;

    // Set up and get a listening socket
    listener = getListenerSocket();
    if (listener == -1) {
		std::cerr << "Error: error getting listening socket\n";
        exit(1);
    }

    // Add the listener to set
	struct pollfd pfd;
    pfd.fd = listener;
    pfd.events = POLLIN; // Report ready to read on incoming connection
	pfds.push_back(pfd); 

    // Main loop
    while(1) {
        // wait indefinitely until 1 or more fds become ready for reading (POLLIN) or other events.
        int pollCount = poll(&pfds[0], pfds.size(), -1);

        if (pollCount == -1) {
            perror("poll");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for(size_t i = 0; i < pfds.size(); i++) {

            // Check if someone's ready to read
            if (pfds[i].revents & (POLLIN | POLLHUP)) { // We got one!!

                if (pfds[i].fd == listener) {
                    // If listener is ready to read, handle new connection

                    addrLen = sizeof remoteAddr;
                    newFd = accept(listener, (struct sockaddr *)&remoteAddr, &addrLen);
                    if (newFd == -1) {
                        perror("accept");
                    } else {
                        
                        // server sends text to every client that connects
                        const char *hello = "Hello from server!";
                        send(newFd, hello, strlen(hello), 0);

						addToPfds(pfds, newFd);

                        printf("pollserver: new connection from %s on "
                            "socket %d\n",
                            inet_ntop(remoteAddr.ss_family,
                                getInAddr((struct sockaddr*)&remoteAddr),
                                remoteIp, INET6_ADDRSTRLEN),
                            newFd);
                        
                        char buffer[30000];
                        recv(newFd, buffer, sizeof(buffer), 0);
						std::cout << buffer << '\n';
                    }
                } 
                else {
                    // If not the listener, we're just a regular client
                    int nBytes = recv(pfds[i].fd, buf, sizeof buf, 0);
                    int senderFd = pfds[i].fd;

                    if (nBytes <= 0) {
                        // Got error or connection closed by client
                        if (nBytes == 0) {
                            // Connection closed
							std::cout << "pollserver: socket " << senderFd << "hung up\n";
                        } else {
                            perror("recv");
                        }
                        close(pfds[i].fd); // Bye!
						delFromPfds(pfds, i);

                        // reexamine the slot we just deleted
                        i--;
                    } 
                    else {
                        // We got some good data from a client

                        for(size_t j = 0; j < pfds.size(); j++) {
                            // Send to everyone!
                            int destFd = pfds[j].fd;

                            // Except the listener and ourselves
                            if (destFd != listener && destFd != senderFd)
                                if (send(destFd, buf, nBytes, 0) == -1)
                                    perror("send");
                        }
                    }
                } 
            } 
        } 
    } 
    
    return 0;
}
