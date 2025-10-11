/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdiomand <mdiomand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 14:17:39 by idioumas          #+#    #+#             */
/*   Updated: 2025/10/11 15:17:22 by mdiomand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRC_Serveur.hpp"
#include <iostream>
#include <cstdlib>

int main(int ac, char **av)
{
    int port = 6667;
    std::string pass = "0000";

    if (ac >= 2) port = std::atoi(av[1]);
    if (ac >= 3) pass = av[2];

    std::cout << "pass = " << pass << std::endl;
    IRC_Serveur irc(port, pass);
    irc.run();
    return 0;
}
