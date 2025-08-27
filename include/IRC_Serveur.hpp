/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Serveur.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idioumas <idioumas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 13:24:03 by idioumas          #+#    #+#             */
/*   Updated: 2025/08/27 16:57:25 by idioumas         ###   ########.fr       */
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
    // vector<Client> client;
    
public:
    IRC_Serveur(int port);
    void run();
    ~IRC_Serveur();
};

IRC_Serveur::IRC_Serveur(int port)
{
    /* Socket creation */
    this->fd_server = socket(AF_INET,SOCK_STREAM,0);
    if (this->fd_server < 0)
    {
        std::cerr << "Error socket creation" << std::endl;
        exit(1);
    }
    // facultatif permet la reconnexion immadiate sur le meme port apres une deconnexion avant le de TIME_WAIT
    int opt = 1;
    setsockopt(this->fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            // Préparer l’adresse
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");// <- Puisque tout se fera en local ou  INADDR_ANY permet d'ecouter sur tous les ports;
    addr.sin_port = htons(port);

    // bind associe le port et l'adresse ip avec le socket (fd)
    // 127.0.0.1:6667"
    if (bind(this->fd_server, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error binding association failed" << std::endl;
        close(fd_server);
        exit(1);
    }
    // listen transforme la socket en serveur 
    // SOMAXCONN constant pour le nombre max de client autorise par l'os
    if (listen(this->fd_server, SOMAXCONN) < 0) {
        std::cerr << "Error listen" << std::endl;
        close(this->fd_server);
        exit(1);
    }
    std::cout << "Serveur IRC en écoute sur le port " << port << "..." << std::endl;
}


void IRC_Serveur::run()
{
    Client client[10];
    
    fd_set master_set, read_set;
    int max_fd = this->fd_server;
    FD_ZERO(&master_set);
    FD_SET(this->fd_server, &master_set);
    
    while (1)
    {
        read_set = master_set;
        if (select(max_fd + 1, &read_set, NULL, NULL, NULL) < 0)
        {
            std::cerr<< "select" << std::endl;
            break;
        }

        for (int fd = 0; fd <= max_fd; fd++)
        {
            if (FD_ISSET(fd, &read_set))
            {
                if (fd == this->fd_server)
                {
                    int client_fd = accept(this->fd_server, NULL, NULL);
                    client[fd].set_id(fd);
                    FD_SET(client_fd, &master_set);

                    if (client_fd > max_fd)
                        max_fd = client_fd;
                    std::cout << "Nouveau client connecté : " << client_fd << std::endl;
                    send(client_fd, "Bienvenue sur le chat IRC !\n", 28, 0);
                }
                else
                {
                    char buffer[512];
                    int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
                    if (bytes <= 0)
                    {
                        std::cout << "Client déconnecté : " << fd << std::endl;
                        close(fd);
                        FD_CLR(fd, &master_set);
                    }
                    else
                    {
                        buffer[bytes - 1] = '\0';
                        std::cout << "Message du client " << client[fd].get_nickname() << " : " << buffer << std::endl;

                        if (!strncmp(buffer, "EXIT", 4))
                        {
                            send(fd, "vous vous etes deconecter !", 27, 0);
                            std::cout << "Client déconnecté : " << fd << std::endl;
                            close(fd);
                            FD_CLR(fd, &master_set);
                        }

                        else if (!strncmp(buffer, "PING", 4))
                            send(fd, "PONG\n", 5, 0);

                        else if (!strncmp(buffer, "NICK ", 5))
                        {
                            send(fd, "vous avez changer de pseudo !\n", 30, 0);
                            std::cout << "Client a changer son pseudo." << std::endl;
                            client[fd].set_nickname(&buffer[5]);
                        }

                        else if (client[fd].get_nickname() == "nickname")
                            send(fd, "veuillier d'abord changer votre pseudo en utulisent la commande 'NICK' suivie de votre pseudo !\n", 96, 0);
                        
                        else if (!strncmp(buffer, "USER ", 5))
                        {
                            send(fd, "vous avez changer de nom d'utilisateur !\n", 41, 0);
                            std::cout << "Client a changer son nom d'utilisateur." << std::endl;
                            client[fd].set_username(&buffer[5]);
                        }
                        
                        else if (client[fd].get_username() == "username")
                            send(fd, "veuillier d'abord changer votre nom d'utilisateur en utulisent la commande 'USER' suivie de votre nom d'utilisateur !\n", 118, 0);
                        
                        else if (!strncmp(buffer, "JOIN ", 5))
                        {
                            if (client[fd].get_chanelname() != "void")
                                std::cout << "Client a quitter le chanel." << std::endl;
                            send(fd, "vous avez changer de canal !\n", 29, 0);
                            client[fd].set_chanelname(&buffer[5]);
                            std::cout << "Client a rejoins le chanel " << &buffer[5] << " ." << std::endl;
                        }

                        else if (client[fd].get_chanelname() == "void")
                            send(fd, "veuillier d'abord enter dans un salon en utulisent la commande 'JOIN' suivie de votre nom du salon !\n", 111, 0);
                        
                        else if (!strncmp(buffer, "PART ", 5))
                        {
                            if (client[fd].get_chanelname() == "void")
                            send(fd, "vous etes dans aucun chanel !\n", 30, 0);
                            else
                            {
                                client[fd].set_chanelname("void");
                                send(fd, "vous avez quitter le canal !\n", 29, 0);
                            }
                        }

                        else if (!strncmp(buffer, "KICK", 4))
                            std::cout << "Client a retirer quelqu'un" << std::endl;

                        else if (!strncmp(buffer, "INVITE", 4))
                            std::cout << "Client a invité quelqu'un" << std::endl;

                        else if (!strncmp(buffer, "TOPIC", 4))
                            std::cout << "Client a changer le theme." << std::endl;

                        else if (!strncmp(buffer, "MODE", 4))
                            std::cout << "Client a ouvert les parametre" << std::endl;

                        else
                        {
                            for (int other_fd = 0; other_fd <= max_fd; other_fd++)
                                if (FD_ISSET(other_fd, &master_set) && client[other_fd].get_chanelname() == client[other_fd].get_chanelname() && other_fd != this->fd_server && other_fd != fd)
                                    send_msg(client[fd].get_nickname(), buffer, other_fd);
                        }
                    }
                }
            }
        }
    }
    close(this->fd_server);
}    
IRC_Serveur::~IRC_Serveur()
{
    std::cout << "test destruction" <<std::endl;   
}
