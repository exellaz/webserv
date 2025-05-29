#include "sockets-polling.h"

// Get sockaddr, IPv4 or IPv6:
void *getInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Return a listening socket
int getListenerSocket(void)
{
    int listener;     // Listening socket descriptor
    int yes = 1;      // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        
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
        return -1;
    }

    freeaddrinfo(ai); // All done with this

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

// Add a new file descriptor to the set
void addToPfds(struct pollfd *pfds[], int newFd, int *fdCount, int *fdSize)
{
    // If we don't have room, add more space in the pfds array
    if (*fdCount == *fdSize) {
        *fdSize *= 2; // Double it

        *pfds = (struct pollfd *)realloc(*pfds, sizeof(**pfds) * (*fdSize));
    }

    (*pfds)[*fdCount].fd = newFd;
    (*pfds)[*fdCount].events = POLLIN; // Check ready-to-read
    (*pfds)[*fdCount].revents = 0;

    (*fdCount)++;
}

// Remove an index from the set
void delFromPfds(struct pollfd pfds[], int i, int *fdCount)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fdCount-1];

    (*fdCount)--;
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

    // Start off with room for 5 connections
    // (We'll realloc as necessary)
    int fdCount = 0;
    int fdSize = 5;
    struct pollfd *pfds = (struct pollfd *)malloc(sizeof *pfds * fdSize);

    // Set up and get a listening socket
    listener = getListenerSocket();

    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    // Add the listener to set
    pfds[0].fd = listener;
    pfds[0].events = POLLIN; // Report ready to read on incoming connection

    fdCount = 1; // For the listener

    // Main loop
    while(1) {
        // wait indefinitely until 1 or more fds become ready for reading (POLLIN) or other events.
        int pollCount = poll(pfds, fdCount, -1);

        if (pollCount == -1) {
            perror("poll");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for(int i = 0; i < fdCount; i++) {

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

                        addToPfds(&pfds, newFd, &fdCount, &fdSize);

                        printf("pollserver: new connection from %s on "
                            "socket %d\n",
                            inet_ntop(remoteAddr.ss_family,
                                getInAddr((struct sockaddr*)&remoteAddr),
                                remoteIp, INET6_ADDRSTRLEN),
                            newFd);
                        
                        char buffer[30000];
                        recv(newFd, buffer, sizeof(buffer), 0);
                        printf("%s\n", buffer);
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
                            printf("pollserver: socket %d hung up\n", senderFd);
                        } else {
                            perror("recv");
                        }

                        close(pfds[i].fd); // Bye!

                        delFromPfds(pfds, i, &fdCount);

                        // reexamine the slot we just deleted
                        i--;

                    } 
                    else {
                        // We got some good data from a client

                        for(int j = 0; j < fdCount; j++) {
                            // Send to everyone!
                            int destFd = pfds[j].fd;

                            // Except the listener and ourselves
                            if (destFd != listener && destFd != senderFd) {
                                if (send(destFd, buf, nBytes, 0) == -1) {
                                    perror("send");
                                }
                            }
                        }
                    }
                } 
            } 
        } 
    } 
    
    return 0;
}
