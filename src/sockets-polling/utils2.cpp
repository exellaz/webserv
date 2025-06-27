#include "../../include/sockets-polling.h"

/**
 * @brief normalize the multiple "/" in the relative uri to one "/"
*/
std::string normalizeSlash(const std::string &relativeUri)
{
    for(size_t i = 0; i < relativeUri.size(); ++i)
    {
        if (relativeUri[i] != '/')
            return relativeUri; // Contains something else, return as is
    }
    return relativeUri.empty() ? "" : "/"; // Only slashes (and not empty)
}

void resolveAliasPath(const std::string& uri, Connection &connection)
{
    const Location location = connection.server.getLocationPath(uri);

    if (!location.getAlias().empty())
    {
        std::cout << "Alias found\n"; ////debug"
        std::string getRelativeUri = uri.substr(location.getLocaPath().length());
        std::string relativeUri = normalizeSlash(getRelativeUri);
        std::cout << "Relative path: " << relativeUri << "\n"; ////debug
        if ((!location.getIndex().empty()) && (relativeUri.empty() || relativeUri == "/"))
        {
            std::cout << GREEN "Alias path with index: " << getFullPath(location.getAlias() + relativeUri + "/") << "\n" RESET; ////debug
            connection.locationPath = getFullPath(location.getAlias() + relativeUri + "/");
        }
    }
}

/**
 * @brief get full path from the uri
*/
std::string resolveHttpPath(const std::string& uri, Server &server)
{
    const Location location = server.getLocationPath(uri);

    if (!location.getAlias().empty())
    {
        std::cout << "Alias found\n"; ////debug"
        std::string getRelativeUri = uri.substr(location.getLocaPath().length());
        std::string relativeUri = normalizeSlash(getRelativeUri);
        std::cout << "Relative path: " << relativeUri << "\n"; ////debug
        if ((!location.getIndex().empty()) && (relativeUri.empty() || relativeUri == "/"))
        {
            std::cout << "Alias path with index: " << getFullPath(location.getAlias() + relativeUri + "/" + location.getIndex()) << "\n"; ////debug
            return getFullPath(location.getAlias() + relativeUri + "/" + location.getIndex());
        }
        else
        {
            std::cout << "Alias path without index: " << getFullPath(location.getAlias() + relativeUri) << "\n"; ////debug
            return getFullPath(location.getAlias() + relativeUri);
        }
    }
    else if (!location.getRoot().empty())
    {
        std::cout << "Root found\n"; ////debug
        std::string getRelativeUri = uri.substr(location.getLocaPath().length());
        std::string relativeUri = normalizeSlash(getRelativeUri);
        if ((!location.getIndex().empty()) && (relativeUri.empty() || relativeUri == "/"))
        {
            std::cout << "Root path with index: " << getFullPath(location.getRoot() + uri + "/" + location.getIndex()) << "\n"; ////debug
            return getFullPath(location.getRoot() + uri + "/" + location.getIndex());
        }
        else
        {
            std::cout << "Root path without index: " << getFullPath(location.getRoot() + relativeUri) << "\n"; ////debug
            return getFullPath(location.getRoot() + relativeUri);
        }
    }
    return "";
}

//bool serveStaticFile(const std::string &httpPath, int clientFd)
//{
//    std::cout << "html file found\n"; ////debug
//    std::ifstream htmlFile(httpPath.c_str());
//    if (!htmlFile)
//        return false;
//    std::string html = readFileToString(htmlFile);
//    std::ostringstream httpSize;
//    httpSize << html.size();
//    std::string httpRes = "HTTP/1.1 200 OK\r\n" //? handle by response
//        "Content-Type: text/html\r\n"
//        "Content-Length: " + httpSize.str() + "\r\n"
//        "\r\n" + html;
//    send(clientFd, httpRes.c_str(), httpRes.size(), 0);
//    return true;
//}

//bool serveAutoIndex(const std::string &httpPath, const std::string &url, int clientFd)
//{
//    std::cout << "file list found\n"; ////debug
//    std::stringstream htmlOutput;
//    htmlOutput << "<html><head><title>Index of " << url << "</title></head><body>";
//    htmlOutput << "<h1>Index of " << url << "</h1><hr><ul>";
//    DIR* dir = opendir(httpPath.c_str());
//    if (!dir)
//        return false;
//    struct dirent* entry;
//    while ((entry = readdir(dir)) != NULL) {
//        std::string name = entry->d_name;
//        if (name == ".") continue;
//        std::string slash = (entry->d_type == DT_DIR) ? "/" : "";
//        std::string url_base = url;
//        if (url_base.empty() || url_base[url_base.size() - 1] != '/')
//            url_base += '/';
//        htmlOutput << "<li><a href=\"" << url_base << name << slash << "\">" << name << slash << "</a></li>";
//    }
//    closedir(dir);
//    htmlOutput << "</ul><hr></body></html>";
//    std::ostringstream httpSize;
//    httpSize << htmlOutput.str().size();
//    std::string httpRes = "HTTP/1.1 200 OK\r\n" //? handle by response
//        "Content-Type: text/html\r\n"
//        "Content-Length: " + httpSize.str() + "\r\n"
//        "\r\n" + htmlOutput.str();
//    send(clientFd, httpRes.c_str(), httpRes.size(), 0);
//    return true;
//}

/**
 * @brief Reads the contents of a directory and generates an HTML index page
*/
std::string readDirectorytoString(const std::string &directoryPath, const std::string& uri)
{
    std::stringstream htmlOutput;
    htmlOutput << "<html><head><title>Index of " << uri << "</title></head><body>";
    htmlOutput << "<h1>Index of " << uri << "</h1><hr><ul>";
    DIR* dir = opendir(directoryPath.c_str());
    if (!dir)
        return "";
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == ".") continue;
        std::string slash = (entry->d_type == DT_DIR) ? "/" : "";
        std::string url_base = uri;;
        if (url_base.empty() || url_base[url_base.size() - 1] != '/')
            url_base += '/';
        htmlOutput << "<li><a href=\"" << url_base << name << slash << "\">" << name << slash << "</a></li>";
    }
    closedir(dir);
    htmlOutput << "</ul><hr></body></html>";
    return htmlOutput.str();
}

/**
 * @brief Get the port number of the socket
 * @note 1. getSockName() retrieves the local address of the socket
 * @note 2. ntohs() converts the port number from network byte order to host byte ( mean from big-endian to 16-bit number)
 * @note 3. sockaddr is used to store the address of the socket
 * @note 4. sockaddr_in is used to store the address of the socket in IPv4 format
*/
// std::string getSocketPortNumber(int fd)
// {
//     std::stringstream intToString;
//     struct sockaddr_storage remoteAddr;
//     socklen_t addrLen = sizeof(remoteAddr);
//     getsockname(fd, (struct sockaddr *)&remoteAddr, &addrLen);
//     int localPort = ntohs(((struct sockaddr_in*)&remoteAddr)->sin_port);
//     intToString << localPort;
//     return (intToString.str());
// }

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