#include "../../include/sockets-polling.h"

//std::string resolveAliasPath(const std::string &url, const Location &location)
//{
//    std::string pathInLocation;
//    std::string fullAliasPath;
//    std::string locationPath = location.locationPath;
//    std::string alias = location.alias;

//    //if locationPath is empty
//    if (url.find(locationPath) == 0)
//        pathInLocation = url.substr(locationPath.length());
//    else
//        pathInLocation = "";
//    //check if alias is empty && if pathInLocation is empty
//    if (!pathInLocation.empty() && pathInLocation[0] != '/')
//        pathInLocation = "/" + pathInLocation;
//    //if location is empty or pathLocation ends with '/'
//    if (pathInLocation.empty() || pathInLocation[pathInLocation.size() - 1] == '/')
//        pathInLocation += location.index;
//    //if alias is not empty && alias end with '/' && pathInLocation starts with '/'
//    if (!alias.empty() && alias[alias.size() - 1] == '/' && pathInLocation[0] == '/')
//        fullAliasPath = alias.substr(0, alias.size() - 1) + pathInLocation;
//    //if alias end is not '/' && pathInLocation does not start with '/'
//    else if (alias[alias.size() - 1] != '/' && pathInLocation[0] != '/')
//        fullAliasPath = alias + "/" + pathInLocation;
//    else
//        fullAliasPath = alias + pathInLocation;
//    return fullAliasPath;
//}

/**
 * @brief get the server by port
*/
Server getServerByPort(const std::vector<Server> &servers, const std::string port)
{
    for (std::vector<Server>::const_iterator it = servers.begin(); it != servers.end(); ++it)
    {
        if (it->getPort() == port)
            return *it;
    }
    return Server();
}

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

/**
 * @brief get full path from the uri
*/
std::string resolveHttpPath(const HttpRequest &request, Server &server)
{
    const Location location = server.getLocationPath(request.getURI());

    if (!location.alias.empty())
    {
        std::cout << "Alias found\n"; ////debug"
        std::string getRelativeUri = request.getURI().substr(location.locationPath.length());
        std::string relativeUri = normalizeSlash(getRelativeUri);
        std::cout << "Relative path: " << relativeUri << "\n"; ////debug
        if ((!location.index.empty()) && (relativeUri.empty() || relativeUri == "/"))
        {
            std::cout << "Alias path with index: " << getFullPath(location.alias + relativeUri + "/" + location.index) << "\n"; ////debug
            return getFullPath(location.alias + relativeUri + "/" + location.index);
        }
        else
        {
            std::cout << "Alias path without index: " << getFullPath(location.alias + relativeUri) << "\n"; ////debug
            return getFullPath(location.alias + relativeUri);
        }
    }
    else if (!location.root.empty())
    {
        std::cout << "Root found\n"; ////debug
        std::string getRelativeUri = request.getURI().substr(location.locationPath.length());
        std::string relativeUri = normalizeSlash(getRelativeUri);
        if ((!location.index.empty()) && (relativeUri.empty() || relativeUri == "/"))
        {
            std::cout << "Root path with index: " << getFullPath(location.root + request.getURI() + "/" + location.index) << "\n"; ////debug
            return getFullPath(location.root + request.getURI() + "/" + location.index);
        }
        else
        {
            std::cout << "Root path without index: " << getFullPath(location.root + relativeUri) << "\n"; ////debug
            return getFullPath(location.root + relativeUri);
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
std::string readDirectorytoString(const std::string &directoryPath, const HttpRequest &request)
{
    std::stringstream htmlOutput;
    htmlOutput << "<html><head><title>Index of " << request.getURI() << "</title></head><body>";
    htmlOutput << "<h1>Index of " << request.getURI() << "</h1><hr><ul>";
    DIR* dir = opendir(directoryPath.c_str());
    if (!dir)
        return "";
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == ".") continue;
        std::string slash = (entry->d_type == DT_DIR) ? "/" : "";
        std::string url_base = request.getURI();;
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
std::string getSocketPortNumber(int fd)
{
    std::stringstream intToString;
    struct sockaddr_storage remoteAddr;
    socklen_t addrLen = sizeof(remoteAddr);
    getsockname(fd, (struct sockaddr *)&remoteAddr, &addrLen);
    int localPort = ntohs(((struct sockaddr_in*)&remoteAddr)->sin_port);
    intToString << localPort;
    return (intToString.str());
}