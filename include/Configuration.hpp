/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sting <sting@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 15:29:35 by welow             #+#    #+#             */
/*   Updated: 2025/05/30 18:02:23 by sting            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

class Location;

class Config
{
	private:
	std::string						_port;           // port number
	std::string						_host;           // host IP address
	std::string						_serverName;     // server name
	std::string						_rootDirectory;  // root directory
	int								_clientMaxSize;  // maximum client request size
	std::map<int, std::string>		_errorPage;      // error pages(error code to file path)
	std::map<std::string, Location>	_location;       // location paths to Location objects
	public:
	Config(std::ifstream &conf);
	~Config();

	std::string								getPort() const;
	std::string								getHost() const;
	std::string								getServerName() const;
	std::string								getRootDirectory() const;
	int										getClientMaxSize() const;
	const std::map<int, std::string>		&getErrorPage() const;
	std::string								getErrorPageByCode(int errorCode) const;
	const std::map<std::string, Location>	&getLocations() const;
	Location								getLocationPath(const std::string &path);
};

class Location
{
	public:
		std::string					locationPath;   // location path
		std::string					index;          // index file
		std::string					rootDirectory;  // root directory for this location
		std::string					alias;		    // alias for this location
		std::vector<std::string>	allowMethods;   // allowed method type (GET, POST)
		std::map<int, std::string>	returnPath;     // path to redirect to for return responses
		int							clientMaxSize;  // maximum client request size
		bool						autoIndex;      // auto generate a directory list if no index file is found

		Location(std::ifstream &conf, const std::string &locName);
		Location();
		~Location();
		std::string getAllowedMethods(size_t index) const;
};

std::ostream &operator<<(std::ostream &cout, const Config &config);

#endif
