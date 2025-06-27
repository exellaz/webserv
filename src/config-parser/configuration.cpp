#include "../../include/Configuration.hpp"

/**
 * @brief Config constructor
*/
//Server::Server(std::istream &conf)
//    : _port(""),
//      _host(""),
//      _serverName(""),
//      _root(""),
//      _clientMaxSize(0),
//      _clientBodyBufferSize(0),
//      _clientHeaderBufferSize(0),
//      _clientTimeout(0),
//      _errorPage()
//{
//    if (conf.peek() == std::ifstream::traits_type::eof())
//        throw std::runtime_error("server file is empty");
//    for (std::string line; std::getline(conf,line);)
//    {
//        line = checkComment(line);
//        if (line.find("listen") != std::string::npos)
//        {
//            std::string listen = line.substr(line.find(' ') + 1,line.find(';') - line.find(' ') - 1);
//            size_t colon = listen.find(':');
//            if (colon != std::string::npos)
//            {
//                this->_host = listen.substr(0, colon);
//                this->_port = listen.substr(colon + 1);
//            }
//            else
//                this->_port = listen.substr(colon + 1);
//        }
//        else if (line.find("server_name") != std::string::npos)
//        {
//            std::string serverName = line.substr(line.find(' ') + 1,line.find(';') - line.find(' ') - 1);
//            this->_serverName = serverName;
//        }
//        else if (line.find("root") != std::string::npos)
//        {
//            std::string rootPath = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
//            this->_root = rootPath;
//        }
//        else if (line.find("allowed_method") != std::string::npos)
//        {
//            std::istringstream iss(line);
//            std::string keyword, method;
//            iss >> keyword; // skip "allowed_method"
//            while (iss >> method)
//            {
//                if (!method.empty() && method[method.size() - 1] == ';') // check if method ends with ';'
//                    method.erase(method.size() - 1); //if not erase the last character
//                if (!method.empty()) // check if method is not empty
//                    this->_allowMethods.push_back(method); //get the current method to the vector
//            }
//        }
//        else if (line.find("client_max_body_size") != std::string::npos)
//        {
//            std::string clientMaxSize = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
//            this->_clientMaxSize = std::strtol(clientMaxSize.c_str(), NULL, 10);
//        }
//        else if (line.find("client_body_buffer_size") != std::string::npos)
//        {
//            std::string clientBodyBufferSize = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
//            this->_clientBodyBufferSize = std::strtol(clientBodyBufferSize.c_str(), NULL, 10);
//        }
//        else if (line.find("client_header_buffer_size") != std::string::npos)
//        {
//            std::string clientHeaderBufferSize = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
//            this->_clientHeaderBufferSize = std::strtol(clientHeaderBufferSize.c_str(), NULL, 10);
//        }
//        else if (line.find("client_timeout") != std::string::npos)
//        {
//            std::string clientTimeout = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
//            this->_clientTimeout = std::strtol(clientTimeout.c_str(), NULL, 10);
//        }
//        else if (line.find("error_page") != std::string::npos)
//        {
//            std::string errorPage = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
//            size_t errorCodePos = errorPage.find(' ');
//            if (errorCodePos != std::string::npos)
//            {
//                int errorCode = std::strtol(errorPage.substr(0, errorCodePos).c_str(), NULL, 10);
//                std::string errorFilePath = ft_trim(errorPage.substr(errorCodePos + 1));
//                this->_errorPage[errorCode] = errorFilePath;
//            }
//        }
//        else if (line.find("location") != std::string::npos)
//        {
//            Location loc(conf, line, getAllowMethods());
//            this->_location[loc.getLocaPath()] = loc;
//        }
//        else if (line.find("}") != std::string::npos)
//            break;
//    }
//}
Server::Server(std::istream &conf)
    : _port(""),
      _host(""),
      _serverName(""),
      _root(""),
      _clientMaxSize(0),
      _clientBodyBufferSize(0),
      _clientHeaderBufferSize(0),
      _clientTimeout(0),
      _errorPage()
{
    if (conf.peek() == std::ifstream::traits_type::eof())
        throw std::runtime_error("server file is empty");

    for (std::string line; std::getline(conf, line);)
    {
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;
        if (keyword == "listen")
        {
            iss >> this->_host;
            if (this->_host[this->_host.size() - 1] == ';')
                this->_host.erase(this->_host.size() - 1); // remove the last character if it is ';' //TODO
            if (this->_host.find(':') != std::string::npos)
            {
                size_t colon = this->_host.find(':');
                this->_port = this->_host.substr(colon + 1);
                this->_host.erase(colon);
            }
            else
                iss >> this->_port;
        }
        else if (keyword == "server_name")
        {
            iss >> this->_serverName;
            if (this->_serverName[this->_serverName.size() - 1] == ';')
                this->_serverName.erase(this->_serverName.size() - 1); // remove the last character if it is ';' //TODO
        }
        else if (keyword == "root")
        {
            iss >> this->_root;
            if (this->_root[this->_root.size() - 1] == ';')
                this->_root.erase(this->_root.size() - 1);
        }
        else if (keyword == "client_max_body_size")
        {
            iss >> this->_clientMaxSize;
        }
        else if (keyword == "client_body_buffer_size")
        {
            iss >> this->_clientBodyBufferSize;
        }
        else if (keyword == "client_header_buffer_size")
        {
            iss >> this->_clientHeaderBufferSize;
        }
        else if (keyword == "client_timeout")
        {
            iss >> this->_clientTimeout;
        }
        else if (keyword == "error_page")
        {
            int errorCode;
            std::string errorFilePath;
            iss >> errorCode >> errorFilePath;
            if (errorFilePath[errorFilePath.size() - 1] == ';') // check if error file path ends with ';'
                errorFilePath.erase(errorFilePath.size() - 1); // remove the last character if it is ';'
            this->_errorPage[errorCode] = errorFilePath;
        }
        else if (keyword == "allowed_method")
        {
            std::string method;
            iss >> method; // read the first method
            if (method[method.size() - 1] == ';') // check if method ends with ';'
                method.erase(method.size() - 1);
            this->_allowMethods.clear(); // clear the previous allowed methods
            if (!method.empty())
                this->_allowMethods.push_back(method); //add the current method to the vector
        }
        else if (keyword == "location")
        {
            std::string locationPath;
            iss >> locationPath; // read the location path
            Location loc(conf, locationPath, this->_allowMethods);
            this->_location[loc.getLocaPath()] = loc; // add the location to the map
        }
        else if (line.find("}") != std::string::npos)
            break;
    }
}


/////////////////////////////////////////////// GETTER /////////////////////////////////////////////////////////////

/**
 * @brief get port number
*/
const std::string	&Server::getPort() const
{
    return (this->_port);
}

/**
 * @brief get ip address or DNS name
*/
const std::string	&Server::getHost() const
{
    return (this->_host);
}

/**
 * @brief get server name
*/
const std::string	&Server::getServerName() const
{
    return (this->_serverName);
}

/**
 * @brief get root directory
*/
const std::string	&Server::getRoot() const
{
    return (this->_root);
}

/**
 * @brief get allowed methods
*/
const std::vector<std::string>	&Server::getAllowMethods() const
{
    return (this->_allowMethods);
}

/**
 * @brief get client max size
*/
int Server::getClientMaxSize() const
{
    return (this->_clientMaxSize);
}

/**
 * @brief get client body buffer size
*/
int	Server::getClientBodyBufferSize() const
{
    return (this->_clientBodyBufferSize);
}

/**
 * @brief get client header buffer size
*/
int	Server::getClientHeaderBufferSize() const
{
    return (this->_clientHeaderBufferSize);
}

/**
 * @brief get large client header buffer number
*/
int	Server::getClientTimeout() const
{
    return (this->_clientTimeout);
}

/**
 * @brief get error log
*/
const std::map<int, std::string>	&Server::getErrorPage() const
{
    return (this->_errorPage);
}

/**
 * @brief get error log by code
 * @param errorCode error code
*/
const std::string &Server::getErrorPageByCode(int errorCode) const
{
    std::map<int, std::string>::const_iterator it = this->_errorPage.find(errorCode);
    if (it != this->_errorPage.end())
        return it->second;
    throw std::runtime_error("error page not found");
}

/**
 * @brief get location
*/
const std::map<std::string, Location>	&Server::getLocations() const
{
    return (this->_location);
}

/**
 * @brief get location contain
 * @param path location path
 * @note 1. if the location is "/", return the location if it matches the path
 * @note 2. if prefix match and (length is equal or next char is '/'), return the location
*/
Location Server::getLocationPath(const std::string &path)
{
    std::string match;
    for (std::map<std::string, Location>::iterator it = this->_location.begin(); it != this->_location.end(); ++it)
    {
        const std::string& loc = it->first;
        if (loc == "/")
        {
            if (loc == path)
                return it->second;
            continue;
        }

        bool prefixMatch = path.compare(0, loc.length(), loc) == 0;
        //bool exactMatch = path.length() == loc.length();
        //bool nextCharSlash = path.length() > loc.length() && path[loc.length()] == '/';
        if (prefixMatch)
                match = loc;
    }
    if (!match.empty())
        return this->_location[match];
    return Location();
}