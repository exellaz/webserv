#include "../../include/sockets-polling.h"

/**
 * @note 1. getSockName() retrieves the local address of the socket
 * @note 2. ntohs() converts the port number from network byte order to host byte ( mean from big-endian to 16-bit number)
 * @note 3. sockaddr is used to store the address of the socket
 * @note 4. sockaddr_in is used to store the address of the socket in IPv4 format
*/
std::pair<std::string, std::string> getIpAndPortFromSocketFd(int fd)
{
    struct sockaddr_storage remoteAddr;
    socklen_t addrLen = sizeof(remoteAddr);
    getsockname(fd, (struct sockaddr *)&remoteAddr, &addrLen);

    int port = ntohs(((struct sockaddr_in*)&remoteAddr)->sin_port);
    std::stringstream portStream;
    portStream << port;

    int ip = ntohl(((struct sockaddr_in*)&remoteAddr)->sin_addr.s_addr);
    std::ostringstream ipStream;
    ipStream << ((ip >> 24) & 0xFF) << "."
                << ((ip >> 16) & 0xFF) << "."
                << ((ip >> 8)  & 0xFF) << "."
                << (ip & 0xFF);

    return std::make_pair(ipStream.str(), portStream.str());
}


Server& getDefaultServerBlockByIpPort(std::pair<std::string, std::string> ipPort, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers)
{
    for (std::map<std::pair<std::string, std::string>, std::vector<Server> >::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        if (it->first == ipPort)
            return *(it->second.begin());
    }
    // if cannot match with `ipPort` // ? will this ever happen?
    return *(servers.begin()->second.begin());
}