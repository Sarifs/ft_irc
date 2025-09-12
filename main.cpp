/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 14:17:39 by idioumas          #+#    #+#             */
/*   Updated: 2025/09/11 22:20:25 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/IRC_Serveur.hpp"

// void handle_sigint(int sig)
// {
//     std::cout << "\nCTRL+C détecté, fermeture propre du client.\n";
//     // -> ici tu peux envoyer "QUIT" au serveur IRC
//     exit(0); 
// }

int main(int ac, char **av)
{
    //signal(SIGPIPE, SIG_IGN);
    //signal(SIGINT, handle_sigint);
    std::string pass = "0000";
    if (ac == 3)
        pass = av[2];
    std::cout << "pass = " << pass << std::endl;
    IRC_Serveur irc(6667, pass);
    irc.run();
    return 0;
}