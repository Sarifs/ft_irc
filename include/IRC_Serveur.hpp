/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Serveur.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:24:03 by idioumas          #+#    #+#             */
/*   Updated: 2025/09/07 15:25:49 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
/*utils*/

#include "Client.hpp"

class IRC_Serveur
{
private:
    IRC_Serveur();
    int fd_server;
    std::vector<int> fds_client;
    std::string passwold;
    
public:
    IRC_Serveur(int port, std::string passwold);
    std::string get_password();
    void run();
    ~IRC_Serveur();
};

