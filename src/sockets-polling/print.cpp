#include "../../include/sockets-polling.h"

void printListeners(std::vector<int>& vec)
{
    std::cout << "\n----Listeners----\n";
    std::vector<int>::const_iterator it = vec.begin();
    for (; it != vec.end(); ++it) {
        std::cout << "fd: " << *it << '\n';
    }
}

void printClients(std::vector<Client>& vec)
{
    std::cout << "\n----Clients----\n";
    std::vector<Client>::const_iterator it = vec.begin();
    for (; it != vec.end(); ++it) {
        std::cout << "fd: " << it->fd << '\n';
    }
}

void printPfds(std::vector<struct pollfd>& vec)
{
    std::cout << "\n----Pfds----\n";
    std::vector<struct pollfd>::const_iterator it = vec.begin();
    for (; it != vec.end(); ++it) {
        std::cout << "fd: " << it->fd << '\n';
    }
}
