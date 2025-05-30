#include "sockets-polling.h"

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

// returns listening socket fd
int setupListeningSocket(std::vector<struct pollfd>& pfds) {

    int listener = getListenerSocket();
    if (listener == -1) {
		std::cerr << "Error: error getting listening socket\n";
        exit(1);
    }

    // Add the listener to set
	struct pollfd pfd;
    pfd.fd = listener;
    pfd.events = POLLIN; // Report ready to read on incoming connection
	pfds.push_back(pfd); 

	return listener;
}
