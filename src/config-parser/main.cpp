/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: welow <welow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 16:14:15 by welow             #+#    #+#             */
/*   Updated: 2025/06/27 08:11:12 by welow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#define BLUE "\033[34m"
#define RESET "\033[0m"

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
        std::map<int, std::vector<Server> > listServers = parseAllServers(configFile);
        for (std::map<int, std::vector<Server> >::iterator it = listServers.begin(); it != listServers.end(); ++it)
        {
            std::cout << BLUE "Port: " << it->first << "\n";
            std::cout << "------------------------\n";
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
		std::cout << "location [/get_output] allow method : " << config.getLocationPath("/youtube").allowMethods[0] << "\n"; //to get allowed method of location path
		std::cout << "location [/youtube] return          : " << config.getLocationPath("/youtube").returnPath[301] << "\n"; //to get return path of location path
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << "\n";
	}
	return 0;
}
