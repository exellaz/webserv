/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sting <sting@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 16:14:15 by welow             #+#    #+#             */
/*   Updated: 2025/06/27 14:02:58 by sting            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#define BLUE "\033[34m"
#define RESET "\033[0m"
#define ORANGE "\033[0;38;5;166m"


int main(int ac, char **av)
{
    std::string configFile;

    if (ac < 2) {
        std::cout << "No config file found. Using default config file.\n";
        (void)av;
        configFile = "../../conf/default.conf";
    }
    else if (ac > 2) {
        std::cout << "Error: Expected 1 config file only.\n";
        return 1;
    }
    else {
        configFile = av[1];
    }

	try
	{
		(void)ac;
		std::ifstream conf(configFile.c_str());
		Server config(conf);

		//visualize
        // std::map<int, std::vector<Server> > listServers = parseAllServers(configFile);
        std::map< std::pair<std::string, std::string> , std::vector<Server> > listServers = parseAllServers(configFile);

        for (std::map< std::pair<std::string, std::string>, std::vector<Server> >::iterator it = listServers.begin(); it != listServers.end(); ++it)
        {
            std::cout << BLUE "Host:: " << it->first.first << ", Port:: " << it->first.second << RESET << "\n";
            std::cout << "------------------------\n";
            std::cout << ORANGE << "SERVER SIZE: " << it->second.size() << RESET << '\n';
            for (std::vector<Server>::iterator serverIt = it->second.begin(); serverIt != it->second.end(); ++serverIt)
            {
                std::cout << *serverIt; //print server information
            }
            std::cout << "------------------------\n\n" RESET;
        }

		//method to get config info
		std::cout << "----- test -----" << "\n";
		std::cout << "host                                : " << config.getHost() << "\n"; //to get server
		std::cout << "location [/] root directory         : " << config.getLocationPath("/").root << "\n"; //to get root directory of location path
		std::cout << "error page (404)                    : " << config.getErrorPageByCode(404) << "\n"; //to get error page by code
        
        // ! SEGFAULT
		// std::cout << "location [/get_output] allow method : " << config.getLocationPath("/youtube").allowMethods[0] << "\n"; //to get allowed method of location path
		
        std::cout << "location [/youtube] return          : " << config.getLocationPath("/youtube").returnPath[301] << "\n"; //to get return path of location path
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << "\n";
	}
	return 0;
}
