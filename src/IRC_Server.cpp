/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 19:45:14 by marvin            #+#    #+#             */
/*   Updated: 2025/08/28 19:45:14 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRC_Serveur.hpp"
#include "../include/Chanel.hpp"

enum Command {
    CMD_EXIT,
    CMD_PING,
    CMD_NICK,
    CMD_USER,
    CMD_JOIN,
    CMD_PART,
    CMD_PRIVMSG,
    CMD_KICK,
    CMD_INVITE,
    CMD_TOPIC,
    CMD_MODE,
    CMD_UNKNOWN
};

Command parse_command(const char *buffer) {
    if (!strncmp(buffer, "EXIT", 4)) return CMD_EXIT;
    if (!strncmp(buffer, "PING", 4)) return CMD_PING;
    if (!strncmp(buffer, "NICK", 4)) return CMD_NICK;
    if (!strncmp(buffer, "USER", 4)) return CMD_USER;
    if (!strncmp(buffer, "JOIN", 4)) return CMD_JOIN;
    if (!strncmp(buffer, "PART", 4)) return CMD_PART;
    if (!strncmp(buffer, "PRIVMSG", 7)) return CMD_PRIVMSG;
    if (!strncmp(buffer, "KICK", 4)) return CMD_KICK;
    if (!strncmp(buffer, "INVITE", 6)) return CMD_INVITE;
    if (!strncmp(buffer, "TOPIC", 5)) return CMD_TOPIC;
    if (!strncmp(buffer, "MODE", 4)) return CMD_MODE;
    return CMD_UNKNOWN;
}

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
    Client client[10000];
    Chanel chanel("test" , "modo");

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
                    client[fd].set_id(client_fd);
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
                    // apprelle le parece de momo
                    else
                    {
                        buffer[bytes - 1] = '\0';
                        std::cout << "Message du client " << client[fd].get_nickname() << " : " << buffer << std::endl;

                        Command cmd = parse_command(buffer);

                        switch (cmd) {
                            case CMD_EXIT:
                                send(fd, "vous vous etes deconecter !", 27, 0);
                                std::cout << "Client déconnecté : " << fd << std::endl;
                                close(fd);
                                FD_CLR(fd, &master_set);
                                break;

                            case CMD_PING:
                                send(fd, "PONG\n", 5, 0);
                                break;

                            case CMD_NICK:
                            {
                                std::string nick(&buffer[5]);
                                nick.erase(nick.find_last_not_of(" \r\n") + 1); // nettoie la fin
                                
                                if (nick.empty())
                                {
                                    send(fd, "Erreur: pseudo invalide\n", 25, 0);
                                }
                                else
                                {
                                    client[fd].set_nickname(nick);
                                    send(fd, "vous avez changer de pseudo !\n", 30, 0);
                                    std::cout << "Client " << fd << " a changé son pseudo en: " << nick << std::endl;
                                }
                                break;
                            }

                            case CMD_USER:
                            {
                                std::string user(&buffer[5]);
                                user.erase(user.find_last_not_of(" \r\n") + 1); // nettoie la fin
                                
                                if (user.empty())
                                {
                                    send(fd, "Erreur: pseudo invalide\n", 25, 0);
                                }
                                else
                                {
                                    client[fd].set_username(user);
                                    send(fd, "vous avez changer de pseudo !\n", 30, 0);
                                    std::cout << "Client " << fd << " a changé son pseudo en: " << user << std::endl;
                                }
                                break;
                            }

                            case CMD_JOIN:
                            {
                                std::string test = chanel.get_name();
                                if (!strncmp(&buffer[5], test.c_str() , 4))
                                {
                                    std::vector<std::string> user = chanel.get_user();
                                    for (size_t i = 0; i < user.size(); i++)
                                    {
                                        if (client[fd].get_username() == user[i])
                                        {
                                            std::cout << "il est dans la liste des user du chanel" << std::endl;
                                        }
                                        else if (i + 1 == user.size())
                                        {
                                            std::cout << "je l'ajoute dans la liste des user du chanel" << std::endl;
                                            chanel.add_user(client[fd].get_username());
                                        }
                                    }
                                }
                                else
                                    std::cout << "chanel introuvable : " << &buffer[5] << "|" << std::endl;
                                break;
                            }

                            case CMD_PART:
                            {
                                std::string test = chanel.get_name();
                                if (!strncmp(&buffer[5], test.c_str() , 4))
                                {
                                    std::vector<std::string> user = chanel.get_user();
                                    for (size_t i = 0; i < user.size(); i++)
                                    {
                                        if (client[fd].get_username() == user[i])
                                        {
                                            std::cout << "je vous sort de la liste des user du chanel" << std::endl;
                                            chanel.del_user(client[fd].get_username());
                                        }
                                        else if (i + 1 == user.size())
                                            std::cout << "vous etes pas dans la liste des user du chanel" << std::endl;
                                    }
                                }
                                else
                                    std::cout << "chanel introuvable : " << &buffer[5] << "|" << std::endl;
                                break;
                            }

                            case CMD_PRIVMSG:
                            {
                                if (buffer[8] == '#')
                                {
                                    std::vector<std::string> users = chanel.get_user();

                                    for (size_t i = 0; i < users.size(); i++)
                                    {
                                        if (client[fd].get_username() == users[i])
                                        {
                                            for (int other_fd = 0; other_fd <= max_fd; other_fd++)
                                            {
                                                for (size_t j = 0; j < users.size(); j++)
                                                {
                                                    if (client[other_fd].get_username() == users[j] && other_fd != fd)
                                                    {
                                                        send_msg(client[fd].get_username(), &buffer[13], other_fd);
                                                        break;
                                                    }
                                                }
                                            }
                                            break;
                                        }
                                    }
                                    std::cout << "message sur un canal" << std::endl;
                                }
                                else
                                {
                                    for (int other_fd = 0; other_fd <= max_fd; other_fd++)
                                    {
                                        std::string name = client[other_fd].get_nickname();
                                        if (!strncmp(&buffer[8], name.c_str(), name.size()))
                                        {
                                            send_msg(client[fd].get_username(), &buffer[name.size() + 9], other_fd);
                                        }
                                    }
                                    std::cout << "message privé" << std::endl;
                                }
                                break;
                            }


                            case CMD_KICK:
                                std::cout << "Client a retirer quelqu'un" << std::endl;
                                break;

                            case CMD_INVITE:
                                std::cout << "Client a invité quelqu'un" << std::endl;
                                break;

                            case CMD_TOPIC:
                                std::cout << "Client a changer le theme." << std::endl;
                                break;

                            case CMD_MODE:
                                std::cout << "Client a ouvert les parametres" << std::endl;
                                break;

                            case CMD_UNKNOWN:
                                for (int other_fd = 0; other_fd <= max_fd; other_fd++) {
                                    if (FD_ISSET(other_fd, &master_set) &&
                                        client[other_fd].get_chanelname() == client[fd].get_chanelname() &&
                                        other_fd != this->fd_server && other_fd != fd)
                                    {
                                        send_msg(client[fd].get_nickname(), buffer, other_fd);
                                    }
                                }
                                break;
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