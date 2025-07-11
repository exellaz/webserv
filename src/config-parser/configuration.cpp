#include "Configuration.h"
#include "http-exception.h"
#include "color.h"

Server::Server(std::istream &conf)
    : _port(""),
      _host(""),
      _serverName(""),
      _root(""),
      _allowMethods(),
      _clientMaxSize(0),
      _clientBodyBufferSize(0),
      _clientHeaderBufferSize(0),
      _clientTimeout(0),
      _errorPage()
{
    if (conf.peek() == std::ifstream::traits_type::eof())
        throw std::runtime_error("server file is empty");
    for (std::string line; std::getline(conf,line);) {
        line = ft_trim(checkComment(line));
        if (line.empty())
            continue;
        if (line == "{" || line == "server {")
            continue;
        if (line == "}")
            break;
        Directive directiveType = getKey(line);
        switch (directiveType) {
            case LISTEN:
            {
                checkValidDirective(line, LISTEN);
                std::string listen = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                size_t colon = listen.find(':');
                if (colon != std::string::npos)
                {
                    this->_host = listen.substr(0, colon);
                    this->_port = checkPort(listen.substr(colon + 1));
                }
                else
                    this->_port = checkPort(listen.substr(colon + 1));
                break;
            }
            case SERVERNAME:
            {
                checkValidDirective(line, SERVERNAME);
                std::string serverName = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                this->_serverName = serverName;
                break;
            }
            case ROOT:
            {
                checkValidDirective(line, ROOT);
                std::string rootPath = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                this->_root = rootPath;
                break;
            }
            case ALLOWED_METHOD:
            {
                checkValidDirective(line, ALLOWED_METHOD);
                this->_allowMethods.clear();
                this->_allowMethods = checkMethod(line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1));
                break;
            }
            case CLIENT_MAX_BODY_SIZE:
            {
                checkValidDirective(line, CLIENT_MAX_BODY_SIZE);
                std::string clientMaxSize = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                this->_clientMaxSize = convertAndCheckNumber(clientMaxSize);
                break;
            }
            case CLIENT_BODY_BUFFER_SIZE:
            {
                checkValidDirective(line, CLIENT_BODY_BUFFER_SIZE);
                std::string clientBodyBufferSize = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                this->_clientBodyBufferSize = convertAndCheckNumber(clientBodyBufferSize);
                break;
            }
            case CLIENT_HEADER_BUFFER_SIZE:
            {
                checkValidDirective(line, CLIENT_HEADER_BUFFER_SIZE);
                std::string clientHeaderBufferSize = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                this->_clientHeaderBufferSize = convertAndCheckNumber(clientHeaderBufferSize);
                break;
            }
            case CLIENT_TIMEOUT:
            {
                checkValidDirective(line, CLIENT_TIMEOUT);
                std::string clientTimeout = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                this->_clientTimeout = convertAndCheckTimeout(clientTimeout);
                break;
            }
            case ERROR_PAGE:
            {
                checkValidDirective(line, ERROR_PAGE);
                std::string errorPage = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                size_t errorCodePos = errorPage.find(' ');
                if (errorCodePos != std::string::npos)
                {
                    int errorCode = convertAndCheckNumber(errorPage.substr(0, errorCodePos));
                    std::string errorFilePath = ft_trim(errorPage.substr(errorCodePos + 1));
                    this->_errorPage[errorCode] = errorFilePath;
                }
                break;
            }
            case LOCATION:
            {
                checkValidDirective(line, LOCATION);
                Location loc(conf, line, getAllowMethods());
                this->_location[loc.getLocaPath()] = loc;
                break;
            }
            default:
                throw std::runtime_error(RED "Unknown directive in server configuration: [" + line + "]" RESET);
        }
    }
}


/////////////////////////////////////////////// GETTER /////////////////////////////////////////////////////////////

/**
 * @brief get port number
*/
const std::string   &Server::getPort() const {
    return (this->_port);
}

/**
 * @brief get ip address or DNS name
*/
const std::string   &Server::getHost() const {
    return (this->_host);
}

/**
 * @brief get server name
*/
const std::string   &Server::getServerName() const {
    return (this->_serverName);
}

/**
 * @brief get root directory
*/
const std::string   &Server::getRoot() const {
    return (this->_root);
}

/**
 * @brief get allowed methods
*/
const std::vector<std::string>  &Server::getAllowMethods() const {
    return (this->_allowMethods);
}

/**
 * @brief get client max size
*/
int Server::getClientMaxSize() const {
    return (this->_clientMaxSize);
}

/**
 * @brief get client body buffer size
*/
int Server::getClientBodyBufferSize() const {
    return (this->_clientBodyBufferSize);
}

/**
 * @brief get client header buffer size
*/
int Server::getClientHeaderBufferSize() const {
    return (this->_clientHeaderBufferSize);
}

/**
 * @brief get large client header buffer number
*/
time_t Server::getClientTimeout() const {
    return (this->_clientTimeout);
}

/**
 * @brief get error log
*/
const std::map<int, std::string>    &Server::getErrorPage() const {
    return (this->_errorPage);
}

/**
 * @brief get error log by code
 * @param errorCode error code
*/
const std::string &Server::getErrorPageByCode(int errorCode) const {
    std::map<int, std::string>::const_iterator it = this->_errorPage.find(errorCode);
    if (it != this->_errorPage.end())
        return it->second;
    throw std::runtime_error("error page not found");
}

/**
 * @brief get location
*/
const std::map<std::string, Location>   &Server::getLocations() const {
    return (this->_location);
}

/**
 * @brief get location contain
 * @param path location path
 * @note 1. if the location is "/", return the location if it matches the path
 * @note 2. if prefix match and (length is equal or next char is '/'), return the location
*/
Location Server::getLocationPath(const std::string &path) {
    if (path.empty() || path[0] != '/')
        throw HttpException(BAD_REQUEST, "Invalid request path");

    std::string bestMatch;
    size_t bestLength = 0;

    for (std::map<std::string, Location>::iterator it = _location.begin(); it != _location.end(); ++it) {
        const std::string& loc = it->first;

        if (loc == path)
            return it->second;

        if (path.compare(0, loc.size(), loc) == 0 && loc.size() > bestLength) {
            bestMatch = loc;
            bestLength = loc.size();
        }
    }

    if (!bestMatch.empty())
        return _location[bestMatch];

    if (_location.count("/"))
        return _location["/"];

    throw HttpException(NOT_FOUND, "No matching location for URI: " + path);
}

void Server::clearServerBlock() {
    this->_port.clear();
    this->_host.clear();
    this->_serverName.clear();
    this->_root.clear();
    this->_allowMethods.clear();
    this->_clientMaxSize = 0;
    this->_clientBodyBufferSize = 0;
    this->_clientHeaderBufferSize = 0;
    this->_clientTimeout = 0;
    this->_errorPage.clear();
    this->_location.clear();
}