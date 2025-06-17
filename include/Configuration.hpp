/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: welow <welow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 15:29:35 by welow             #+#    #+#             */
/*   Updated: 2025/06/17 13:16:30 by welow            ###   ########.fr       */
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
		std::string					_port;           // port number
		std::string					_host;           // host IP address
		std::string					_serverName;     // server name
		std::string					_root;  // root directory
		std::vector<std::string>	_allowMethods;   // allowed method type (GET, POST)
		int							_clientMaxSize;  // maximum client request size
		int							_clientBodyBufferSize; //client body buffer size
		int							_clientHeaderBufferSize; //client header buffer size
		int							_largeClientHeaderBufferNumber; //number of buffer for large client header (eg: user agent, cookies)
		int							_largeClientHeaderBufferSize; //size of buffer for large client header (eg: user agent, cookies)
		std::map<int, std::string>	_errorPage;      // error pages(error code to file path)
		std::map<std::string, Location>	_location;       // location paths to Location obje
	public:
		Config() {}; // Default constructor
		Config(std::istream &conf);
		~Config() {};

		const std::string						&getPort() const;
		const std::string						&getHost() const;
		const std::string						&getServerName() const;
		const std::string						&getRoot() const;
		const std::vector<std::string>			&getAllowMethods() const;
		int										getClientMaxSize() const;
		int										getClientBodyBufferSize() const;
		int										getClientHeaderBufferSize() const;
		int										getLargeClientHeaderBufferNumber() const;
		int										getLargeClientHeaderBufferSize() const;
		const std::map<int, std::string>		&getErrorPage() const;
		const std::string						&getErrorPageByCode(int errorCode) const;
		const std::map<std::string, Location>	&getLocations() const;
		Location								getLocationPath(const std::string &path);
};

class Location
{
	public:
		std::string					locationPath;   // location path
		std::string					index;          // index file
		std::string					root;  // root directory for this location
		std::string					alias;		    // alias for this location
		std::vector<std::string>	allowMethods;   // allowed method type (GET, POST)
		std::map<int, std::string>	returnPath;     // path to redirect to for return responses
		int							clientMaxSize;  // maximum client request size
		bool						autoIndex;      // auto generate a directory list if no index file is found
		std::string					cgi_path;       // cgi path for this location

		Location(std::istream &conf, const std::string &locName);
		Location() {};
		~Location() {};
		std::string getAllowedMethods(size_t index) const;
};

std::vector<Config> parseAllServers(const std::string &filename);
std::ostream &operator<<(std::ostream &cout, const Config &config);

#endif
