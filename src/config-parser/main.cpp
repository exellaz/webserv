/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: welow <welow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 16:14:15 by welow             #+#    #+#             */
/*   Updated: 2025/05/29 17:41:16 by welow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"

int main(int ac, char **av)
{
	try
	{
		(void)ac;
		std::ifstream conf(av[1]);
		Config config(conf);

		//visualize
		std::cout << config << "\n";

		//method to get config info
		std::cout << "----- test -----" << "\n";
		std::cout << config.getHost() << "\n"; //to get server
		std::cout << config.getLocationPath("/api")->cgiPath << "\n"; //to get location
		std::cout << config.getErrorLogByCode(500) << "\n"; //to get error log
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << "\n";
	}
	return 0;
}