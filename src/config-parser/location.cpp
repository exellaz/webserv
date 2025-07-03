#include "Configuration.hpp"

#define RED "\033[31m"
#define RESET "\033[0m"

Location::Location(std::istream &conf, const std::string &locName, const std::vector<std::string> & allowMethods)
    : _locaPath(""),
      _index(""),
      _root(""),
      _alias(""),
      _allowMethods(allowMethods),
      _returnPath(),
      _clientMaxSize(0),
      _autoIndex(false),
      _cgi_path(false),
      _allowUpload(false)
{
    this->_locaPath = extractLine(locName);
    for(std::string line; std::getline(conf, line);)
    {
        line = ft_trim(checkComment(line));
        if (line.empty())
            continue;
        if (line == "}")
            break;
        Directive directiveType = getKey(line);
        switch (directiveType)
        {
            case AUTO_INDEX:
            {
                checkValidDirective(line, AUTO_INDEX);
                std::string autoIndex = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                if (autoIndex == "on")
                    this->_autoIndex = true;
                break;
            }
            case INDEX:
            {
                checkValidDirective(line, INDEX);
                this->_index = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                break;
            }
            case ROOT:
            {
                checkValidDirective(line, ROOT);
                this->_root = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                break;
            }
            case ALIAS:
            {
                checkValidDirective(line, ALIAS);
                this->_alias = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                break;
            }
            case ALLOWED_METHOD:
            {
                checkValidDirective(line, ALLOWED_METHOD);
                this->_allowMethods.clear(); // clear the previous allowed methods
                this->_allowMethods = checkMethod(line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1));
                break;
            }
            case RETURN_PATH:
            {
                checkValidDirective(line, RETURN_PATH);
                std::string returnPath = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                size_t space = returnPath.find(' ');
                if (space != std::string::npos)
                {
                    int errorCode = checkNumber(returnPath.substr(0, space));
                    std::string path = ft_trim(returnPath.substr(space + 1));
                    if (path[0] == '"' && path[path.size() - 1] == '"')
                        path = path.substr(1, path.size() - 2); // remove quotes
                    this->_returnPath[errorCode] = path;
                }
                break;
            }
            case CLIENT_MAX_BODY_SIZE:
            {
                checkValidDirective(line, CLIENT_MAX_BODY_SIZE);
                std::string clientMaxSize = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                this->_clientMaxSize = checkNumber(clientMaxSize);
                break;
            }
            case CGI_PATH:
            {
                checkValidDirective(line, CGI_PATH);
                std::string cgiPath = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                if (cgiPath == "on")
                    this->_cgi_path = true;
                break;
            }
            case ALLOW_UPLOAD:
            {
                checkValidDirective(line, ALLOW_UPLOAD);
                std::string allowUpload = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
                if (allowUpload == "on")
                    this->_allowUpload = true;
                break;
            }
            default:
                throw std::runtime_error(RED "Unknown directive in location block: [" + line + "]" RESET);
        }
    }
}

/////////////////////////////////////////////// GETTER /////////////////////////////////////////////////////////////

void    Location::setAllowMethod(const std::vector<std::string> &methods)
{
	this->_allowMethods = methods;
}

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

bool    Location::getCgiPath() const
{
    return this->_cgi_path;
}

bool    Location::getAllowUpload() const
{
    return this->_allowUpload;
}

void Location::clearLocationBlock()
{
	this->_locaPath.clear();
	this->_index.clear();
	this->_root.clear();
	this->_alias.clear();
	this->_allowMethods.clear();
	this->_returnPath.clear();
	this->_clientMaxSize = 0;
	this->_autoIndex = false;
	this->_cgi_path = false;
	this->_allowUpload = false;
}
