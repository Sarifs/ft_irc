/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 14:17:39 by idioumas          #+#    #+#             */
/*   Updated: 2025/10/10 19:13:46 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRC_Serveur.hpp"

int main(int ac, char **av)
{
    std::string pass = "0000";
    if (ac == 3)
        pass = av[2];
    std::cout << "pass = " << pass << std::endl;
    IRC_Serveur irc(6667, pass);
    irc.run();
    return 0;
}