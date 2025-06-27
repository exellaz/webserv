#include "Configuration.hpp"

Location::Location(std::istream &conf, const std::string &locName, const std::vector<std::string> & allowMethods)
    : _locaPath(""),
      _index(""),
      _root(""),
      _alias(""),
      _allowMethods(allowMethods),
      _returnPath(),
      _clientMaxSize(0),
      _autoIndex(false),
      _allowUpload(false)
{
    this->_locaPath = ft_trim(locName);
    for(std::string line; std::getline(conf, line);)
    {
        line = checkComment(line);
        if (line.find("autoindex") != std::string::npos)
        {
            std::string autoIndex = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
            if (autoIndex == "on")
                this->_autoIndex = true;
        }
        else if (line.find("index") != std::string::npos)
            this->_index = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
        else if (line.find("root") != std::string::npos)
            this->_root = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
        else if (line.find("alias") != std::string::npos)
            this->_alias = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
        else if (line.find("allowed_method") != std::string::npos)
        {
            this->_allowMethods.clear();
            std::istringstream iss(line);
            std::string keyword, method;
            iss >> keyword;
            while (iss >> method)
            {
                if (isupper(method[0]) == false)
                    throw std::runtime_error("allowed_method must be uppercase");
                if (!method.empty() && method[method.size() - 1] == ';')
                    method.erase(method.size() - 1);
                if (!method.empty())
                    this->_allowMethods.push_back(method);
            }
        }
        else if (line.find("return") != std::string::npos)
        {
            std::string returnPath = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
            size_t space = returnPath.find(' ');
            if (space != std::string::npos)
            {
                int errorCode = std::strtol(returnPath.substr(0, space).c_str(), NULL, 10);
                std::string path = ft_trim(returnPath.substr(space + 1));
                this->_returnPath[errorCode] = path;
            }
        }
        else if (line.find("client_max_size") != std::string::npos)
        {
            std::string clientMaxSize = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
            this->_clientMaxSize = std::strtol(clientMaxSize.c_str(), NULL, 10);
        }
        else if (line.find("cgi_path") != std::string::npos)
            this->_cgi_path = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
        else if (line.find("allow_upload") != std::string::npos)
        {
            std::string allowUpload = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
            if (allowUpload == "on")
                this->_allowUpload = true;
        }
        else if (line.find("}") != std::string::npos)
            break;
    }
}

/////////////////////////////////////////////// GETTER /////////////////////////////////////////////////////////////

const std::string   &Location::getLocaPath() const
{
    return this->_locaPath;
}

const std::string    &Location::getIndex() const
{
    return this->_index;
}

const std::string    &Location::getRoot() const
{
    return this->_root;
}

const std::string    &Location::getAlias() const
{
    return this->_alias;
}

const std::vector<std::string> &Location::getAllowMethods() const
{
    return this->_allowMethods;
}

const std::map<int, std::string> &Location::getReturnPath() const
{
    return this->_returnPath;
}

int    Location::getClientMaxSize() const
{
    return this->_clientMaxSize;
}

bool    Location::getAutoIndex() const
{
    return this->_autoIndex;
}

const std::string    &Location::getCgiPath() const
{
    return this->_cgi_path;
}

bool    Location::getAllowUpload() const
{
    return this->_allowUpload;
}
