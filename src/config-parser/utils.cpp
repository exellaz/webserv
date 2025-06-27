#include "Configuration.hpp"

/**
 * @brief print server information
*/
std::ostream &operator<<(std::ostream &cout, const Server &server)
{
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

    for (std::map<int, std::string>::const_iterator it = server.getErrorPage().begin(); it != server.getErrorPage().end(); ++it)
    {
        cout << "error_log      : [" << it->first << "] [" << it->second << "]\n";
    }
    std::cout << "\n";

        if (!server.getAllowMethods().empty())
    {
        cout << "allow_methods  : ";
        for (std::vector<std::string>::const_iterator methodIt = server.getAllowMethods().begin();
                methodIt != server.getAllowMethods().end(); ++methodIt)
        {
            cout << "[" << *methodIt << "]";
        }
        cout << "\n\n";
    }

    for (std::map<std::string, Location>::const_iterator it = server.getLocations().begin();
            it != server.getLocations().end(); ++it)
    {
        const Location &loc = it->second;

        if (!loc.getLocaPath().empty())
            cout << "location        : [" << loc.getLocaPath() << "]\n";
        if (!loc.getIndex().empty())
            cout << "index           : [" << loc.getIndex() << "]\n";
        if (!loc.getRoot().empty())
            cout << "root_directory  : [" << loc.getRoot() << "]\n";
        if (!loc.getAlias().empty())
            cout << "alias_directory : [" << loc.getAlias() << "]\n";
        if (!loc.getAllowMethods().empty())
        {
            cout << "allow_methods   : ";
            for (std::vector<std::string>::const_iterator methodIt = loc.getAllowMethods().begin();
                    methodIt != loc.getAllowMethods().end(); ++methodIt)
            {
                cout << "[" << *methodIt << "]";
            }
            cout << "\n";
        }
        if (!loc.getReturnPath().empty())
        {
            cout << "return_path     : ";
            for (std::map<int, std::string>::const_iterator retIt = loc.getReturnPath().begin();
                    retIt != loc.getReturnPath().end(); ++retIt)
            {
                cout << "[" << retIt->first << "] [" << retIt->second << "]";
            }
            cout << "\n";
        }
        if (loc.getClientMaxSize() != 0)
            cout << "client_max_size : [" << loc.getClientMaxSize() << "]\n";
        if (loc.getAutoIndex() == true)
            cout << "list_directory  : [on]\n";
        if (!loc.getCgiPath().empty())
            cout << "cgi_path        : [" << loc.getCgiPath() << "]\n";
        if (loc.getAllowUpload() == true)
            cout << "allow_upload    : [on]\n";
        std::cout << "\n";
    }
    return cout;
}

///////////////////////////////////////////// HELPER FUNCTION ///////////////////////////////////////////////////////////////

std::map< std::pair<std::string, std::string> , std::vector<Server> > parseAllServers(const std::string &filename)
{
    // std::map<int, std::vector<Server> > listServers;
    std::map< std::pair<std::string, std::string> , std::vector<Server> > listServers;
    std::ifstream conf(filename.c_str());
    std::stringstream serverBlock;

    if (!conf.is_open())
        throw std::runtime_error("all server: failed to open configuration file");

    for (std::string line; std::getline(conf, line);)
    {
        if (line.find("server") != std::string::npos && line.find("{") != std::string::npos)
        {
            serverBlock << line << "\n";
            int braceCount = 1; //look for the next "}"
            while (braceCount != 0 && std::getline(conf, line)) {
                if (line.find('{') != std::string::npos) braceCount++;
                if (line.find('}') != std::string::npos) braceCount--;
                serverBlock << line << "\n"; // if not keep parse the line
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
std::string ft_trim(const std::string &str)
{
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
std::string	checkComment(const std::string &line)
{
    size_t comment_pos = line.find('#');
    if (comment_pos != std::string::npos)
        return line.substr(0, comment_pos);
    return line;
}

/**
 * @brief extract the line between 'line' and '{'
 * @param line line
 * @return extract line
*/
std::string	extractLine(const std::string &line)
{
    std::istringstream iss(line);
    std::string keyword, path;
    iss >> keyword >> path;
    return ft_trim(path);
}