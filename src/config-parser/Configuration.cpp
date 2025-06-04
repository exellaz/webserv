/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: welow <welow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 15:31:13 by welow             #+#    #+#             */
/*   Updated: 2025/06/04 16:37:08 by welow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Configuration.hpp"

static std::string ft_trim(const std::string &str);
static void skipBlock(std::ifstream &conf);
static std::string	checkComment(const std::string &line);
static std::string	extractLine(const std::string &line);

/**
 * @brief Config constructor
*/
Config::Config(std::ifstream &conf)
	: _port(""),
	  _host(""),
	  _serverName(""),
	  _rootDirectory(""),
	  _clientMaxSize(0),
	  _clientBodyBufferSize(0),
	  _clientHeaderBufferSize(0),
	  _largeClientHeaderBufferNumber(0),
	  _largeClientHeaderBufferSize(0),
	  _errorPage()
{
	if (!conf.is_open())
		throw std::runtime_error("config file not found or cannot open");
	else if (conf.peek() == std::ifstream::traits_type::eof())
		throw std::runtime_error("config file is empty");
	for (std::string line; std::getline(conf,line);)
	{
		line = checkComment(line);
		if (line.find("listen") != std::string::npos)
		{
			std::string listen = line.substr(line.find(' ') + 1,line.find(';') - line.find(' ') - 1);
			size_t colon = listen.find(':');
			if (colon != std::string::npos)
			{
				this->_host = listen.substr(0, colon);
				_port = listen.substr(colon + 1);
			}
			else
				_port = listen.substr(colon + 1);
		}
		else if (line.find("server_name") != std::string::npos)
		{
			std::string serverName = line.substr(line.find(' ') + 1,line.find(';') - line.find(' ') - 1);
			this->_serverName = serverName;
		}
		else if (line.find("root") != std::string::npos)
		{
			std::string rootPath = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
			this->_rootDirectory = rootPath;
		}
		else if (line.find("client_max_body_size") != std::string::npos)
		{
			std::string clientMaxSize = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
			this->_clientMaxSize = std::strtol(clientMaxSize.c_str(), NULL, 10);
		}
		else if (line.find("client_body_buffer_size") != std::string::npos)
		{
			std::string clientBodyBufferSize = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
			this->_clientBodyBufferSize = std::strtol(clientBodyBufferSize.c_str(), NULL, 10);
		}
		else if (line.find("client_header_buffer_size") != std::string::npos)
		{
			std::string clientHeaderBufferSize = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
			this->_clientHeaderBufferSize = std::strtol(clientHeaderBufferSize.c_str(), NULL, 10);
		}
		else if (line.find("large_client_header_buffers") != std::string::npos)
		{
			std::string numBuffer = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
			std::string bufferSize = numBuffer.substr(numBuffer.find(' ') + 1);
			int numBuffers = std::strtol(numBuffer.c_str(), NULL, 10);
			int sizeBuffer = std::strtol(bufferSize.c_str(), NULL, 10);
			this->_largeClientHeaderBufferNumber = numBuffers;
			this->_largeClientHeaderBufferSize = sizeBuffer;
		}
		else if (line.find("error_page") != std::string::npos)
		{
			std::string errorPage = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
			size_t errorCodePos = errorPage.find(' ');
			if (errorCodePos != std::string::npos)
			{
				int errorCode = std::strtol(errorPage.substr(0, errorCodePos).c_str(), NULL, 10);
				std::string errorFilePath = ft_trim(errorPage.substr(errorCodePos + 1));
				this->_errorPage[errorCode] = errorFilePath;
			}
		}
		else if (line.find("location") != std::string::npos)
		{
			Location loc(conf, line);
			this->_location[loc.locationPath] = loc;
		}
		else if (line.find("}") != std::string::npos)
			break;
	}
}

/**
 * @brief Location constructor
*/
Location::Location(std::ifstream &conf, const std::string &locName)
	: locationPath(""),
	  index(""),
	  rootDirectory(""),
	  alias(""),
	  allowMethods(),
	  returnPath(),
	  clientMaxSize(0),
	  autoIndex(false)
{
	this->locationPath = extractLine(locName);
	for(std::string line; std::getline(conf, line);)
	{
		line = checkComment(line);
		if (line.find("autoindex") != std::string::npos)
		{
			std::string autoIndex = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
			if (autoIndex == "on")
				this->autoIndex = true;
			else if (autoIndex == "off")
				this->autoIndex = false;
			else
				this->autoIndex = false;
		}
		else if (line.find("index") != std::string::npos)
			this->index = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
		else if (line.find("root") != std::string::npos)
		{
			this->rootDirectory = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
			this->rootDirectory.append(this->locationPath);
		}
		else if (line.find("alias") != std::string::npos)
			this->alias = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
		else if (line.find("limit_except") != std::string::npos)
		{
			std::string allowMethod = extractLine(line);
			std::istringstream iss(allowMethod);
			for (std::string method; iss >> method;)
			{
				if (method != "GET" && method != "POST")
					throw std::runtime_error("invalid method");
				this->allowMethods.push_back(method);
			}
			skipBlock(conf);
		}
		else if (line.find("return") != std::string::npos)
		{
			std::string returnPath = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
			size_t space = returnPath.find(' ');
			if (space != std::string::npos)
			{
				int errorCode = std::strtol(returnPath.substr(0, space).c_str(), NULL, 10);
				std::string path = ft_trim(returnPath.substr(space + 1));
				this->returnPath[errorCode] = path;
			}
		}
		else if (line.find("client_max_size") != std::string::npos)
		{
			std::string clientMaxSize = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
			this->clientMaxSize = std::strtol(clientMaxSize.c_str(), NULL, 10);
		}
		else if (line.find("}") != std::string::npos)
			break;
	}
}

/**
 * @brief get port number
*/
std::string	Config::getPort() const
{
	return (this->_port);
}

/**
 * @brief get ip address or DNS name
*/
std::string	Config::getHost() const
{
	return (this->_host);
}

/**
 * @brief get server name
*/
std::string	Config::getServerName() const
{
	return (this->_serverName);
}

/**
 * @brief get root directory
*/
std::string	Config::getRootDirectory() const
{
	return (this->_rootDirectory);
}

/**
 * @brief get client max size
*/
int Config::getClientMaxSize() const
{
	return (this->_clientMaxSize);
}

/**
 * @brief get client body buffer size
*/
int	Config::getClientBodyBufferSize() const
{
	return (this->_clientBodyBufferSize);
}

/**
 * @brief get client header buffer size
*/
int	Config::getClientHeaderBufferSize() const
{
	return (this->_clientHeaderBufferSize);
}

/**
 * @brief get large client header buffer number
*/
int	Config::getLargeClientHeaderBufferNumber() const
{
	return (this->_largeClientHeaderBufferNumber);
}

/**
 * @brief get large client header buffer size
*/
int Config::getLargeClientHeaderBufferSize() const
{
	return (this->_largeClientHeaderBufferSize);
}

/**
 * @brief get location
*/
const std::map<std::string, Location>	&Config::getLocations() const
{
	return (this->_location);
}

/**
 * @brief get error log
*/
const std::map<int, std::string>	&Config::getErrorPage() const
{
	return (this->_errorPage);
}

/**
 * @brief get location contain
 * @param path location path
*/
Location Config::getLocationPath(const std::string &path)
{
	std::map<std::string, Location>::iterator it = this->_location.find(path);
	if (it != this->_location.end())
		return it->second;
	throw std::runtime_error("location not found");
}

/**
 * @brief get error log by code
 * @param errorCode error code
*/
std::string Config::getErrorPageByCode(int errorCode) const
{
	std::map<int, std::string>::const_iterator it = this->_errorPage.find(errorCode);
	if (it != this->_errorPage.end())
		return it->second;
	throw std::runtime_error("error page not found");
}

/**
 * @brief get allowed methods by index
*/
std::string Location::getAllowedMethods(size_t index) const
{
	if (index <= this->allowMethods.size())
		return this->allowMethods[index];
	throw std::runtime_error("method unavailable");
}

/**
 * @brief print configuration information
*/
std::ostream &operator<<(std::ostream &cout, const Config &config)
{
	if (config.getPort().empty())
		cout << "port           : [" << config.getPort() << "]\n";
	if (!config.getHost().empty())
		cout << "host           : [" << config.getHost() << "]\n";
	if (!config.getServerName().empty())
		cout << "server_name    : [" << config.getServerName() << "]\n";
	if (!config.getRootDirectory().empty())
		cout << "root_directory : [" << config.getRootDirectory() << "]\n";
	if (config.getClientMaxSize() != 0)
		cout << "client_max_size: [" << config.getClientMaxSize() << "]\n";
	if (config.getClientBodyBufferSize() != 0)
		cout << "client_body_buffer_size: [" << config.getClientBodyBufferSize() << "]\n";
	if (config.getClientHeaderBufferSize() != 0)
		cout << "client_header_buffer_size: [" << config.getClientHeaderBufferSize() << "]\n";
	cout << "large_client_header_buffers: ";
	if (config.getLargeClientHeaderBufferNumber() != 0)
		cout << "[" << config.getLargeClientHeaderBufferNumber() << "] [" << config.getLargeClientHeaderBufferSize() << "]\n";
	for (std::map<int, std::string>::const_iterator it = config.getErrorPage().begin();
			it != config.getErrorPage().end(); ++it)
	{
		cout << "error_log      : [" << it->first << "] [" << it->second << "]\n";
	}
	for (std::map<std::string, Location>::const_iterator it = config.getLocations().begin();
			it != config.getLocations().end(); ++it)
	{
		const Location &loc = it->second;

		if (!loc.locationPath.empty())
			cout << "	location        : [" << loc.locationPath << "]\n";
		if (!loc.index.empty())
			cout << "	files           : [" << loc.index << "]\n";
		if (!loc.rootDirectory.empty())
			cout << "	root_directory  : [" << loc.rootDirectory << "]\n";
		if (!loc.alias.empty())
			cout << "	alias_directory : [" << loc.alias << "]\n";
		for (std::vector<std::string>::const_iterator methodIt = loc.allowMethods.begin();
				methodIt != loc.allowMethods.end(); ++methodIt)
		{
			cout << "	allow_method    : [" << *methodIt << "]\n";
		}
		for (std::map<int, std::string>::const_iterator retIt = loc.returnPath.begin();
				retIt != loc.returnPath.end(); ++retIt)
		{
			cout << "	return_path     : [" << retIt->first << "] [" << retIt->second << "]\n";
		}
		if (loc.clientMaxSize != 0)
			cout << "	client_max_size : [" << loc.clientMaxSize << "]\n";
		if (loc.autoIndex)
			cout << "	list_directory  : [on]\n";
		cout << "	-------------------------------------------------\n";
	}
	return cout;
}

Config::~Config() {}
Location::Location() {}
Location::~Location() {}

/**
 * @brief trim front and back whitespace from a string
 * @param str string that i want to trim
 * @return trimmed string
*/
static std::string ft_trim(const std::string &str)
{
	size_t first = str.find_first_not_of(" \t\n\r\f\v");
	if (first == std::string::npos)
		return "";
	size_t last = str.find_last_not_of(" \t\n\r\f\v");
	return str.substr(first, (last - first + 1));
}

/**
 * @brief skip block in config file
 * @param conf config file
 * @note 1. if found '{', increment braceCount (but it already incremented by 1)
 * @note 2. if found '}', decrement braceCount (so it will skip the whole block)
*/
static void skipBlock(std::ifstream &conf) {
	int braceCount = 1;
	std::string line;
	while (std::getline(conf, line)) {
		if (line.find('{') != std::string::npos)
			braceCount++;
		if (line.find('}') != std::string::npos)
			braceCount--;
		if (braceCount == 0)
			break;
	}
}

/**
 * @brief check the line had comment or not
 * @param line string
 * @return line if no comment, otherwise return line before comment
*/
static std::string	checkComment(const std::string &line)
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
static std::string	extractLine(const std::string &line)
{
	size_t	start = line.find(' ') + 1;
	size_t	end = line.find('{');
	if (start == 0 || end == 0 || start >= end)
		throw std::runtime_error("invalid line");
	return ft_trim(line.substr(start, end - start));
}