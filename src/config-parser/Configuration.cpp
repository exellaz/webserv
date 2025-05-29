/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: welow <welow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 15:31:13 by welow             #+#    #+#             */
/*   Updated: 2025/05/29 17:41:53 by welow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"

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

std::string	check_comment(const std::string &line)
{
	size_t comment_pos = line.find('#');
	if (comment_pos != std::string::npos)
		return line.substr(0, comment_pos);
	return line;
}

/**
 * @brief Config constructor
*/
Config::Config(std::ifstream &conf)
	: _port(0),
	  _host(""),
	  _serverName(""),
	  _rootDirectory("")
{
	if (!conf.is_open())
		throw std::runtime_error("Configuration file not found or cannot be opened");
	else if (conf.peek() == std::ifstream::traits_type::eof())
		throw std::runtime_error("Configuration file is empty");
	for (std::string line; std::getline(conf,line);)
	{
		line = check_comment(line);
		if (line.find("port") != std::string::npos)
		{
			std::string portNB = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
			this->_port = std::strtol(portNB.c_str(), NULL, 10);
		}
		else if (line.find("host") != std::string::npos)
		{
			std::string hostName = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
			this->_host = hostName;
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
		else if (line.find("error_log") != std::string::npos)
		{
			std::string errorLog = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
			size_t errorCodePos = errorLog.find(' ');
			if (errorCodePos != std::string::npos)
			{
				int errorCode = std::strtol(errorLog.substr(0, errorCodePos).c_str(), NULL, 10);
				std::string errorFilePath = ft_trim(errorLog.substr(errorCodePos + 1));
				this->_errorLog[errorCode] = errorFilePath;
			}
		}
		else if (line.find("location") != std::string::npos)
		{
			Location loc(conf, line);
			this->_location[loc.locationPath] = loc;
		}
	}
}

/**
 * @brief Location constructor
*/
Location::Location(std::ifstream &conf, const std::string &locName)
	: locationPath(""),
	  index(""),
	  rootDirectory(""),
	  allowMethods(""),
	  cgiPath(""),
	  cgiExtension(""),
	  clientMaxSize(0),
	  autoIndex(false)
{
	unsigned int start_pos = locName.find(' ') + 1;
	unsigned int end_pos = locName.find("{");
	std::string trimLocName = ft_trim(locName.substr(start_pos, end_pos - start_pos));
	this->locationPath = trimLocName;

	for(std::string line; std::getline(conf, line);)
	{
		line = check_comment(line);
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
		else if (line.find("root_directory") != std::string::npos)
			this->rootDirectory = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
		else if (line.find("allow_method") != std::string::npos)
			this->allowMethods = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
		else if (line.find("cgi_path") != std::string::npos)
			this->cgiPath = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
		else if (line.find("cgi_extension") != std::string::npos)
			this->cgiExtension = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
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
unsigned int	Config::getPort() const
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
 * @brief get location
*/
const std::map<std::string, Location>	&Config::getLocations() const
{
	return (this->_location);
}

/**
 * @brief get error log
*/
const std::map<int, std::string>	&Config::getErrorLog() const
{
	return (this->_errorLog);
}

/**
 * @brief get location contain
 * @param path location path
*/
Location *Config::getLocationPath(const std::string &path)
{
	std::map<std::string, Location>::iterator it = this->_location.find(path);
	if (it != this->_location.end())
		return &(it->second);
	throw std::runtime_error("Location not found");
}

/**
 * @brief get error log by code
 * @param errorCode error code
*/
std::string Config::getErrorLogByCode(int errorCode)
{
	std::map<int, std::string>::const_iterator it = this->_errorLog.find(errorCode);
	if (it != this->_errorLog.end())
		return it->second;
	throw std::runtime_error("Error log not found for code");
}

/**
 * @brief print configuration information
*/
std::ostream &operator<<(std::ostream &cout, const Config &config)
{
	cout << "port           : [" << (config.getPort() == 0 ? 0 : config.getPort()) << "]\n";
	cout << "host           : [" << (config.getHost().empty() ? "-" : config.getHost()) << "]\n";
	cout << "server_name    : [" << (config.getServerName().empty() ? "-" : config.getServerName() )<< "]\n";
	cout << "root_directory : [" << (config.getRootDirectory().empty() ? "-" : config.getRootDirectory()) << "]\n";
	for (std::map<int, std::string>::const_iterator it = config.getErrorLog().begin();
			it != config.getErrorLog().end(); ++it)
	{
		cout << "error_log      : [" << it->first << "] [" << it->second << "]\n";
	}
	for (std::map<std::string, Location>::const_iterator it = config.getLocations().begin();
			it != config.getLocations().end(); ++it)
	{
		cout << "	location        : [" << (it->second.locationPath.empty() ? "-" : it->second.locationPath) << "]\n";
		cout << "	index           : [" << (it->second.index.empty() ? "-" : it->second.index) << "]\n";
		cout << "	root_directory  : [" << (it->second.rootDirectory.empty() ? "-" : it->second.rootDirectory) << "]\n";
		cout << "	allow_methods   : [" << (it->second.allowMethods.empty() ? "-" : it->second.allowMethods) << "]\n";
		cout << "	client_max_size : [" << it->second.clientMaxSize << "]\n";
		cout << "	autoindex       : [" << (it->second.autoIndex ? "on" : "off") << "]\n";
		cout << "	cgi_path        : [" << (it->second.cgiPath.empty() ? "-" : it->second.cgiPath) << "]\n";
		cout << "	cgi_extension   : [" << (it->second.cgiExtension.empty() ? "-" : it->second.cgiExtension) << "]\n";
		cout << "	-------------------------------------------------\n";
	}
	return cout;
}