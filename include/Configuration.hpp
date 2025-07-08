#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
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
        std::string                 _port;
        std::string                 _host;
        std::string                 _serverName;
        std::string                 _root;
        std::vector<std::string>    _allowMethods;
        int                         _clientMaxSize;
        int                         _clientBodyBufferSize;
        int                         _clientHeaderBufferSize;
        int                         _clientTimeout;
        std::map<std::string, Location>	_location;
    public:
        Server() {};
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
        const std::map<std::string, Location>   &getLocations() const;
        Location                                getLocationPath(const std::string &path);
};

class Location
{
    private:
        std::string                 _locaPath;
        std::string                 _index;
        std::string                 _root;
        std::string                 _alias;
        std::vector<std::string>	_allowMethods;
        std::map<int, std::string>  _returnPath;
        int                         _clientMaxSize;
        bool                        _autoIndex;
        bool                        _cgiPath;
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
