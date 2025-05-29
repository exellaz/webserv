/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: welow <welow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 15:29:35 by welow             #+#    #+#             */
/*   Updated: 2025/05/29 17:39:03 by welow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>

class Location
{
	public:
		std::string	locationPath;   // location path
		std::string	index;          // index file
		std::string	rootDirectory;  // root directory for this location
		std::string	allowMethods;   // allowed method type (GET, POST)
		std::string	cgiPath;        // path to CGI scripts
		std::string	cgiExtension;   // CGI file extension type
		int			clientMaxSize;  // maximum client request size
		bool		autoIndex;      // auto generate a directory list if no index file is found

		Location(std::ifstream &conf, const std::string &locName);
		Location();
		~Location();
};

class Config
{
	private:
		unsigned int					_port;            // port number
		std::string						_host;            // ip address or DNS name
		std::string						_serverName;      // server name
		std::string						_rootDirectory;   // root directory
		std::map<int, std::string>		_errorLog;      // error pages map (error code to file path)
		std::map<std::string, Location>	_location;// map of location paths to Location objects
	public:
		Config(std::ifstream &conf);
		~Config();

		const std::map<std::string, Location>	&getLocations() const;
		const std::map<int, std::string>		&getErrorLog() const;
		unsigned int							getPort() const;
		std::string								getHost() const;
		std::string								getServerName() const;
		std::string								getRootDirectory() const;
		Location								*getLocationPath(const std::string &path);
		std::string								getErrorLogByCode(int errorCode);
};

std::ostream &operator<<(std::ostream &cout, const Config &config);

#endif