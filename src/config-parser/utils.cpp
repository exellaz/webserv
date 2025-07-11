#include "Configuration.h"
#include <cerrno>

#define RED "\033[31m"
#define RESET "\033[0m"

std::map< std::pair<std::string, std::string> , std::vector<Server> > parseAllServers(const std::string &filename) {
    std::map< std::pair<std::string, std::string> , std::vector<Server> > listServers;
    std::ifstream conf(filename.c_str());
    std::stringstream serverBlock;

    if (!conf.is_open())
        throw std::runtime_error("all server: failed to open configuration file");

    for (std::string line; std::getline(conf, line);) {
        if (line.find("server") != std::string::npos && line.find("{") != std::string::npos) {
            serverBlock << line << "\n";
            int braceCount = 1;
            while (braceCount != 0 && std::getline(conf, line)) {
                if (line.find('{') != std::string::npos)
                    braceCount++;
                if (line.find('}') != std::string::npos)
                    braceCount--;
                serverBlock << line << "\n";
            }
            Server server(serverBlock);
            std::string host = server.getHost();
            std::string port = server.getPort();

            std::pair<std::string, std::string> key = std::make_pair(host, port);
            listServers[key].push_back(server);
        }
    }
    conf.close();
    return listServers;
}

/**
 * @brief trim front and back whitespace from a string
 * @param str string that i want to trim
 * @return trimmed string
*/
std::string ft_trim(const std::string &str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

/**
 * @brief check the line had comment or not
 * @param line string
 * @return line if no comment, otherwise return line before comment
*/
std::string	checkComment(const std::string &line) {
    size_t comment_pos = line.find('#');
    if (comment_pos != std::string::npos)
        return line.substr(0, comment_pos);
    return line;
}

/**
 * @brief extract the location value from the line
 * @param line line
 * @return extract line
*/
std::string	extractLocationValue(const std::string &line) {
    std::istringstream iss(line);
    std::string keyword, path;
    iss >> keyword >> path;
    return ft_trim(path);
}

/**
 * @brief check the method is valid
*/
std::vector<std::string> checkMethod(std::string allowMethod)
{
    std::vector<std::string> methods;
    std::istringstream iss(allowMethod);
    std::string method;
    while (iss >> method) {
        if (method[method.size() - 1] == ';') // check if method ends with ';'
            method.erase(method.size() - 1); //if not erase the last character
        if (isupper(method[0]) == false)
            throw std::runtime_error(RED "allowed_method must be uppercase" RESET);
        if (method != "GET" && method != "POST" && method != "DELETE" && method != "PUT")
            throw std::runtime_error(RED "Invalid allowed method in configuration" RESET);
        if (!method.empty()) // check if method is not empty
            methods.push_back(method); //get the current method to the vector
    }
    return methods;
}

/**
 * @brief check the port number
*/
std::string checkPort(std::string port)
{
    if (port.empty())
        throw std::runtime_error(RED "port is empty" RESET);
    if (isDigitsOnly(port) == false)
        throw std::runtime_error(RED "port is not a number" RESET);
    int portNum = std::strtol(port.c_str(), NULL, 10);
    if (portNum < 1 || portNum > 65535)
        throw std::runtime_error(RED "port number is out of range (1-65535)" RESET);
    return port;
}

time_t convertAndCheckTimeout(std::string number)
{
    if (number.empty())
        throw std::runtime_error(RED "client timeout is empty" RESET);
    if (isDigitsOnly(number) == false)
        throw std::runtime_error(RED "client timeout is not a number" RESET);
    errno = 0;
    long long nb = std::strtoll(number.c_str(), NULL, 10);
    if (errno == ERANGE || std::numeric_limits<time_t>::min() > nb || std::numeric_limits<time_t>::max() < nb)
                    throw std::runtime_error(RED "client timeout exceed numeric limit" RESET);
    return (static_cast<time_t>(nb));
}

/**
 * @brief check if the number
*/
int convertAndCheckNumber(std::string number)
{
    if (number.empty())
        throw std::runtime_error(RED "this is empty" RESET);
    if (isDigitsOnly(number) == false)
        throw std::runtime_error(RED "this is not a number" RESET);
    long nb = std::strtol(number.c_str(), NULL, 10);
    if (INT_MIN > nb || INT_MAX < nb)
        throw std::runtime_error(RED "this had exceed numeric limit" RESET);
    return (static_cast<int>(nb));
}

/**
 * @brief get the directive type
*/
Directive getKey(const std::string &line)
{
    std::istringstream iss(line);
    std::string key;
    iss >> key;
    if (key == "listen")
        return LISTEN;
    else if (key == "server_name")
        return SERVERNAME;
    else if (key == "root")
        return ROOT;
    else if (key == "allowed_method")
        return ALLOWED_METHOD;
    else if (key == "client_max_body_size")
        return CLIENT_MAX_BODY_SIZE;
    else if (key == "client_body_buffer_size")
        return CLIENT_BODY_BUFFER_SIZE;
    else if (key == "client_header_buffer_size")
        return CLIENT_HEADER_BUFFER_SIZE;
    else if (key == "client_timeout")
        return CLIENT_TIMEOUT;
    else if (key == "error_page")
        return ERROR_PAGE;
    else if (key == "location")
        return LOCATION;
    else if (key == "location_path")
        return LOCATION_PATH;
    else if (key == "index")
        return INDEX;
    else if (key == "alias")
        return ALIAS;
    else if (key == "return")
        return RETURN_PATH;
    else if (key == "autoindex")
        return AUTO_INDEX;
    else if (key == "allow_upload")
        return ALLOW_UPLOAD;
    else if (key == "cgi_path")
        return CGI_PATH;
    else
        return UNKNOWN;
}

/**
 * @brief check if the directive is valid
 * @note 1. getline(iss, rest, ';') => read theiss output and store to rest until ';' is found
*/
void checkValidDirective(const std::string &line, Directive directiveType) {
    std::string rest;
    std::istringstream iss(line);
    std::getline(iss, rest, ';');
    if (rest.empty())
        throw std::runtime_error(RED "invalid directive: [" + line + "]" RESET);
    std::istringstream rest_iss(rest);
    std::string key;
    rest_iss >> key;
    if (getKey(key) == ERROR_PAGE) {
        int errorCode;
        std::string value;
        if (!(rest_iss >> errorCode >> value))
            throw std::runtime_error(RED "missing error code or file path in error_page directive: [" + line + "]" RESET);
        std::string extra_value;
        if (rest_iss >> extra_value)
            throw std::runtime_error(RED "extra value found in error_page directive: [" + line + "]" RESET);
        return ;
    }
    if (getKey(key) == RETURN_PATH) {
        int statusCode;
        std::string value;
        if (!(rest_iss >> statusCode))
            throw std::runtime_error(RED "missing status code in return directive: [" + line + "]" RESET);
        std::getline(rest_iss, value);
        value = ft_trim(value);
        if (value.empty())
            throw std::runtime_error(RED "missing value in return directive: [" + line + "]" RESET);
        if (value[0] == '"') {
            if (value[value.size() - 1] != '"')
                throw std::runtime_error(RED "missing closing quote in return directive: [" + line + "]" RESET);
            size_t open_quote = value.find('"');
            size_t close_quote = value.rfind('"');
            if (open_quote != 0 || close_quote != value.size() - 1 || value.find('"', 1) != close_quote)
                throw std::runtime_error(RED "extra value found in return directive: [" + line + "]" RESET);
        } else {
            std::stringstream iss(value);
            std::string str, extra_str;
            iss >> str;
            if (iss >> extra_str)
                throw std::runtime_error(RED "extra value found in unquoted return directive: [" + line + "]" RESET);
        }
        return;
    }
    if (getKey(key) == ALLOWED_METHOD) {
        std::string value;
        if (!(rest_iss >> value))
            throw std::runtime_error(RED "missing allowed method in allowed_method directive: [" + line + "]" RESET);
        return ;
    }
    if (getKey(key) == AUTO_INDEX || getKey(key) == ALLOW_UPLOAD || getKey(key) == CGI_PATH) {
        std::string value;
        if (!(rest_iss >> value))
            throw std::runtime_error(RED "missing directive value: [" + line + "]" RESET);
        if (value != "on" && value != "off")
            throw std::runtime_error(RED "invalid directive value, must be 'on' or 'off': [" + line + "]" RESET);
        return ;
    }
    if (getKey(key) == LOCATION) {
        std::string key;
        std::string brace;
        if (!(rest_iss >> key))
            throw std::runtime_error(RED "missing location path in location directive: [" + line + "]" RESET);
        if (!(rest_iss >> brace) || brace != "{")
            throw std::runtime_error(RED "invalid location directive: [" + line + "]" RESET);
    }
    else if (getKey(key) == directiveType) {
        std::string value;
        std::string extra_value;
        if (!(rest_iss >> value))
            throw std::runtime_error(RED "missing directive value: [" + line + "]" RESET);
        if (rest_iss >>extra_value)
            throw std::runtime_error(RED "extra value found in directive: [" + line + "]" RESET);
        return ;
    }
    else
        throw std::runtime_error(RED "unknown directive: [" + line + "]" RESET);
}

Server& getDefaultServerBlockByIpPort(std::pair<std::string, std::string> ipPort, std::map< std::pair<std::string, std::string> , std::vector<Server> >& servers)
{
    for (std::map<std::pair<std::string, std::string>, std::vector<Server> >::iterator it = servers.begin(); it != servers.end(); ++it) {
        if (it->first == ipPort)
            return *(it->second.begin());
    }
    return *(servers.begin()->second.begin());
}

///////////////////////////////////////////// DEBUG FUNCTION ///////////////////////////////////////////////////////////////

/**
 * @brief print server information
*/
std::ostream &operator<<(std::ostream &cout, const Server &server) {
    if (!server.getPort().empty())
        cout << "port           : [" << server.getPort() << "]\n";
    if (!server.getHost().empty())
        cout << "host           : [" << server.getHost() << "]\n";
    if (!server.getServerName().empty())
        cout << "server_name    : [" << server.getServerName() << "]\n\n";

    if (!server.getRoot().empty())
        cout << "root_directory : [" << server.getRoot() << "]\n\n";

    if (server.getClientMaxSize() != 0)
        cout << "client_max_size          : [" << server.getClientMaxSize() << "]\n";
    if (server.getClientBodyBufferSize() != 0)
        cout << "client_body_buffer_size  : [" << server.getClientBodyBufferSize() << "]\n";
    if (server.getClientHeaderBufferSize() != 0)
        cout << "client_header_buffer_size: [" << server.getClientHeaderBufferSize() << "]\n";
    if (server.getClientTimeout() != 0)
        cout << "client_timeout: [" << server.getClientTimeout() << "]\n\n";

    for (std::map<int, std::string>::const_iterator it = server.getErrorPage().begin(); it != server.getErrorPage().end(); ++it) {
        cout << "error_log      : [" << it->first << "] [" << it->second << "]\n";
    }
    std::cout << "\n";

    if (!server.getAllowMethods().empty()) {
        cout << "allow_methods  : ";
        for (std::vector<std::string>::const_iterator methodIt = server.getAllowMethods().begin();
                methodIt != server.getAllowMethods().end(); ++methodIt) {
            cout << "[" << *methodIt << "]";
        }
        cout << "\n\n";
    }

    for (std::map<std::string, Location>::const_iterator it = server.getLocations().begin();
            it != server.getLocations().end(); ++it) {
        const Location &loc = it->second;

        if (!loc.getLocaPath().empty())
            cout << "location        : [" << loc.getLocaPath() << "]\n";
        if (!loc.getIndex().empty())
            cout << "index           : [" << loc.getIndex() << "]\n";
        if (!loc.getRoot().empty())
            cout << "root_directory  : [" << loc.getRoot() << "]\n";
        if (!loc.getAlias().empty())
            cout << "alias_directory : [" << loc.getAlias() << "]\n";
        if (!loc.getAllowMethods().empty()) {
            cout << "allow_methods   : ";
            for (std::vector<std::string>::const_iterator methodIt = loc.getAllowMethods().begin();
                    methodIt != loc.getAllowMethods().end(); ++methodIt) {
                cout << "[" << *methodIt << "]";
            }
            cout << "\n";
        }
        if (!loc.getReturnPath().empty()) {
            cout << "return_path     : ";
            for (std::map<int, std::string>::const_iterator retIt = loc.getReturnPath().begin();
                    retIt != loc.getReturnPath().end(); ++retIt) {
                cout << "[" << retIt->first << "] [" << retIt->second << "]";
            }
            cout << "\n";
        }
        if (loc.getClientMaxSize() != 0)
            cout << "client_max_size : [" << loc.getClientMaxSize() << "]\n";
        if (loc.getAutoIndex() == true)
            cout << "list_directory  : [on]\n";
        if (loc.getCgiPath() == true)
            cout << "cgi_path        : [on]\n";
        if (loc.getAllowUpload() == true)
            cout << "allow_upload    : [on]\n";
        std::cout << "\n";
    }
    return cout;
}
