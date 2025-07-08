#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
//#include <cstdlib>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "utils.h"
#include <utility>

enum Directive
{
    LISTEN,
    SERVERNAME,
    ROOT,
    ALLOWED_METHOD,
    CLIENT_MAX_BODY_SIZE,
    CLIENT_BODY_BUFFER_SIZE,
    CLIENT_HEADER_BUFFER_SIZE,
    CLIENT_TIMEOUT,
    ERROR_PAGE,
    LOCATION,
    LOCATION_PATH,
    INDEX,
    ALIAS,
    RETURN_PATH,
    AUTO_INDEX,
    ALLOW_UPLOAD,
    CGI_PATH,
    UNKNOWN
};

class Location;

class Server
{
    private:
        std::string                 _port;           // port number
        std::string                 _host;           // host IP address
        std::string                 _serverName;     // server name
        std::string                 _root;  // root directory
        std::vector<std::string>    _allowMethods;   // allowed method type (GET, POST)
        int                         _clientMaxSize;  // maximum client request size
        int                         _clientBodyBufferSize; //client body buffer size
        int                         _clientHeaderBufferSize; //client header buffer size
        int                         _clientTimeout; //size of buffer for large client header (eg: user agent, cookies)
        std::map<int, std::string>  _errorPage;      // error pages(error code to file path)
        std::map<std::string, Location>	_location;       // location paths to Location obje
    public:
        Server() {}; // Default constructor
        Server(std::istream &conf);
        ~Server() {};

        const std::string                       &getPort() const;
        const std::string                       &getHost() const;
        const std::string                       &getServerName() const;
        const std::string                       &getRoot() const;
        const std::vector<std::string>          &getAllowMethods() const;
        int                                     getClientBodyBufferSize() const;
        int                                     getClientHeaderBufferSize() const;
        int                                     getClientTimeout() const;
        int                                     getClientMaxSize() const;
        const std::map<int, std::string>        &getErrorPage() const;
        const std::string                       &getErrorPageByCode(int errorCode) const;
        const std::map<std::string, Location>   &getLocations() const;
        Location                                getLocationPath(const std::string &path);
};

class Location
{
    private:
        std::string                 _locaPath;   // location path
        std::string                 _index;          // index file
        std::string                 _root;  // root directory for this location
        std::string                 _alias;     // alias for this location
        std::vector<std::string>	_allowMethods;   // allowed method type (GET, POST)
        std::map<int, std::string>  _returnPath;     // path to redirect to for return responses
        int                         _clientMaxSize;  // maximum client request size
        bool                        _autoIndex;      // auto generate a directory list if no index file is found
        bool                        _cgiPath;       // cgi path for this location
        bool                        _allowUpload;

    public:
        void setAllowMethod(const std::vector<std::string> &methods);
        const std::string           &getLocaPath() const;
        const std::string           &getIndex() const;
        const std::string           &getRoot() const;
        const std::string           &getAlias() const;
        const std::vector<std::string> &getAllowMethods() const;
        const std::map<int, std::string> &getReturnPath() const;
        int                         getClientMaxSize() const;
        bool                        getAutoIndex() const;
        bool                        getCgiPath() const;
        bool                        getAllowUpload() const;

        Location(std::istream &conf, const std::string &locName, const std::vector<std::string> & defaultMethods);
        Location() {};
        ~Location() {};

        void clearLocationBlock();
};

std::ostream &operator<<(std::ostream &cout, const Server &server);
std::map< std::pair<std::string, std::string> , std::vector<Server> > parseAllServers(const std::string &filename);
std::string ft_trim(const std::string &str);
std::string	checkComment(const std::string &line);
std::string	extractLocationValue(const std::string &line);
std::vector<std::string> checkMethod(std::string allowMethod);
std::string checkPort(std::string port);
int checkNumber(std::string number);
Directive getKey(const std::string &line);
void checkValidDirective(const std::string &line, Directive directiveType);
Server& getDefaultServerBlockByIpPort(std::pair<std::string, std::string> ipPort,
                                      std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers);

#endif
