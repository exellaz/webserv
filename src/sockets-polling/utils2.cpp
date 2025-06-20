#include "../../include/sockets-polling.h"

std::string resolveAliasPath(const std::string &url, const Location &location)
{
    std::string pathInLocation;
    std::string fullAliasPath;
    std::string locationPath = location.locationPath;
    std::string alias = location.alias;

    //if locationPath is empty
    if (url.find(locationPath) == 0)
        pathInLocation = url.substr(locationPath.length());
    else
        pathInLocation = "";
    //check if alias is empty && if pathInLocation is empty
    if (!pathInLocation.empty() && pathInLocation[0] != '/')
        pathInLocation = "/" + pathInLocation;
    //if location is empty or pathLocation ends with '/'
    if (pathInLocation.empty() || pathInLocation[pathInLocation.size() - 1] == '/')
        pathInLocation += location.index;
    //if alias is not empty && alias end with '/' && pathInLocation starts with '/'
    if (!alias.empty() && alias[alias.size() - 1] == '/' && pathInLocation[0] == '/')
        fullAliasPath = alias.substr(0, alias.size() - 1) + pathInLocation;
    //if alias end is not '/' && pathInLocation does not start with '/'
    else if (alias[alias.size() - 1] != '/' && pathInLocation[0] != '/')
        fullAliasPath = alias + "/" + pathInLocation;
    else
        fullAliasPath = alias + pathInLocation;
    return fullAliasPath;
}

//std::string readFileToString (std::ifstream &file)
//{
//    std::string content;
//    for (std::string line; std::getline(file, line);)
//        content += line + "\n";
//    return content;
//}

Server getServerConfigByPort(const std::vector<Server> &servers, const std::string port)
{
    for (std::vector<Server>::const_iterator it = servers.begin(); it != servers.end(); ++it)
    {
        if (it->getPort() == port)
            return *it;
    }
    return Server();
}

std::string resolveHttpPath(const HttpRequest &request, Server &server)
{
    const Location location = server.getLocationPath(request.getURI());

    if (!location.alias.empty())
    {
        std::cout << "Alias found\n"; ////debug"
        return getFullPath(resolveAliasPath(request.getURI(), location));
    }
    else if (!location.root.empty())
    {
        std::cout << "Root found\n"; ////debug
        std::cout << "Root path: " << getFullPath(request.getURI() + "/" + location.index) << "\n";
        return getFullPath(request.getURI() + "/" + location.index);
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

std::string extractPortFromHost(const std::string &host)
{
    size_t colon = host.find(':');
    if (colon == std::string::npos)
        return "";
    return host.substr(colon + 1);
}