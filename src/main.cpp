/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoumare <asoumare@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 14:17:39 by idioumas          #+#    #+#             */
/*   Updated: 2025/11/01 19:44:28 by asoumare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRC_Serveur.hpp"

#include <iostream>
#include <string>
#include <stdexcept>
#include <limits>
#include <signal.h>



#include <sstream>

static bool parsePort(const char* s, int& out_port) {
    std::istringstream iss(s);
    int p = 0;
    if (!(iss >> p)) {
        return false;  // conversion échouée
    }
    // vérifier qu'il n'y a rien après le nombre
    char c;
    if (iss >> c) {
        return false;  // caractères invalides après le nombre
    }
    if (p < 1 || p > 65535) {
        return false;  // port hors plage
    }
    out_port = p;
    return true;
}


int main(int ac, char** av)
{
    const int DEFAULT_PORT = 6667;
    const std::string DEFAULT_PASS = "0000";

    int port = DEFAULT_PORT;
    std::string pass = DEFAULT_PASS;
    signal(SIGPIPE, SIG_IGN);

    if (ac == 1) {
        // pas d'argument : on garde les valeurs par défaut
    } else if (ac == 2) {
        // 1 argument : c'est le port
        if (!parsePort(av[1], port)) {
            std::cerr << "Port invalide : '" << av[1] << "'. Utiliser un entier entre 1 et 65535.\n";
            return 1;
        }
    } else if (ac == 3) {
        // 2 arguments : port et pass
        if (!parsePort(av[1], port)) {
            std::cerr << "Port invalide : '" << av[1] << "'. Utiliser un entier entre 1 et 65535.\n";
            return 1;
        }
        pass = av[2];
    } else {
        std::cerr << "Usage:\n";
        std::cerr << "  " << av[0] << "                # lance avec port 6667 et pass 0000\n";
        std::cerr << "  " << av[0] << " <port>         # lance avec <port> et pass 0000\n";
        std::cerr << "  " << av[0] << " <port> <pass>  # lance avec <port> et <pass>\n";
        return 1;
    }

    std::cout << "port = " << port << ", pass = " << pass << std::endl;

    // Instancie ton serveur avec le port et le mot de passe choisis
    IRC_Serveur irc(port, pass);
    irc.run();

    return 0;
}
