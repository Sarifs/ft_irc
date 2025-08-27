/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Serveur.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idioumas <idioumas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:24:03 by idioumas          #+#    #+#             */
/*   Updated: 2025/08/27 15:46:44 by idioumas         ###   ########.fr       */
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


class IRC_Serveur
{
private:
    IRC_Serveur();
    int fd_serveur;
    std::vector<int> fds_client;
    // vector<Client> client;
    
public:
    IRC_Serveur(int port);
    void run();
    ~IRC_Serveur();
};

IRC_Serveur::IRC_Serveur(int port)
{
    /* Socket creation */
    this->fd_serveur = socket(AF_INET,SOCK_STREAM,0);
    if (this->fd_serveur < 0)
    {
        std::cerr << "Error socket creation" << std::endl;
        exit(1);
    }
    // facultatif permet la reconnexion immadiate sur le meme port apres une deconnexion avant le de TIME_WAIT
    // int opt = 1;
    // setsockopt(this->fd_serveur, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            // Préparer l’adresse
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");// <- Puisque tout se fera en local ou  INADDR_ANY permet d'ecouter sur tous les ports;
    addr.sin_port = htons(port);

    // bind associe le port et l'adresse ip avec le socket (fd)
    // 127.0.0.1:6667"
    if (bind(this->fd_serveur, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error binding association failed" << std::endl;
        close(fd_serveur);
        exit(1);
    }
    // listen transforme la socket en serveur 
    // SOMAXCONN constant pour le nombre max de client autorise par l'os
    if (listen(this->fd_serveur, SOMAXCONN) < 0) {
        std::cerr << "Error listen" << std::endl;
        close(this->fd_serveur);
        exit(1);
    }
    std::cout << "Serveur IRC en écoute sur le port " << port << "..." << std::endl;
}


void IRC_Serveur::run()
{
    for (int i = 0; i < 10; i ++);
}    
IRC_Serveur::~IRC_Serveur()
{
    std::cout << "test destruction" <<std::endl;   
}
