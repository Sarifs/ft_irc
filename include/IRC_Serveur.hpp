/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Serveur.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdiomand <mdiomand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:24:03 by idioumas          #+#    #+#             */
/*   Updated: 2025/10/11 15:19:32 by mdiomand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef IRC_SERVEUR_HPP
#define IRC_SERVEUR_HPP

#include <string>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <csignal>
#include <iostream>

#include "Client.hpp"
#include "Chanel.hpp"
#include "Parsing.hpp"

class IRC_Serveur {
public:
    IRC_Serveur(int port, const std::string &pass);
    ~IRC_Serveur();

    void run();
    void stop();
    std::string get_password() const;

private:
    int                      fd_server;
    std::string              passwold;

    std::vector<Client>      clients;   // propriétaire des clients
    std::vector<Chanel>      chanels;   // propriétaire des channels

    bool                     running;   // C++98 (pas d'atomic)
};

#endif
