/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: welow <welow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 16:14:15 by welow             #+#    #+#             */
/*   Updated: 2025/06/20 16:03:10 by welow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"

int main(int ac, char **av)
{
	try
	{
		(void)ac;
		std::ifstream conf(av[1]);
		Server config(conf);

		//visualize
		std::cout << config << "\n";

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
