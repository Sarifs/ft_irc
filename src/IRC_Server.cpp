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
#include "../include/Parsing.hpp"

enum Command {
    CMD_EXIT,
    CMD_PING,
    CMD_NICK,
    CMD_NONICK,
    CMD_NOUSER,
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

Command parse_command(const char *buffer, Client client)
{
    std::string nick = client.get_nickname();
    std::string user = client.get_username();

    if (!strcmp(buffer, "EXIT")) return CMD_EXIT;
    if (!strcmp(buffer, "PING")) return CMD_PING;
    if (!strcmp(buffer, "NICK")) return CMD_NICK;
    if (!strcmp(nick.c_str(), "")) return CMD_NONICK;
    if (!strcmp(buffer, "USER")) return CMD_USER;
    if (!strcmp(user.c_str(), "")) return CMD_NOUSER;
    if (!strcmp(buffer, "JOIN")) return CMD_JOIN;
    if (!strcmp(buffer, "PART")) return CMD_PART;
    if (!strcmp(buffer, "PRIVMSG")) return CMD_PRIVMSG;
    if (!strcmp(buffer, "KICK")) return CMD_KICK;
    if (!strcmp(buffer, "INVITE")) return CMD_INVITE;
    if (!strcmp(buffer, "TOPIC")) return CMD_TOPIC;
    if (!strcmp(buffer, "MODE")) return CMD_MODE;
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
    IRCMessage IRC;
    std::vector<Client> clients;
    Chanel chanel("test" , "modo");

    fd_set master_set, read_set;

    int max_fd = this->fd_server;
    Client srvr(this->fd_server);
    clients.push_back(srvr);
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

        for (size_t i = 0; i < clients.size(); i++)
        {
            if (FD_ISSET(clients[i].get_fd_client(), &read_set))
            {
                if (clients[i].get_fd_client() == this->fd_server)
                {
                    int client_fd = accept(this->fd_server, NULL, NULL);
                    if (client_fd >= 0)
                    {                        
                        Client new_client(client_fd);
                        clients.push_back(new_client);
                    }
                    else if (client_fd < 0) 
                        std::cout << "Error accepte"<<std::endl;
                    FD_SET(client_fd, &master_set);

                    if (client_fd > max_fd)
                        max_fd = client_fd;
                    std::cout << "Nouveau client connecté : " << client_fd << std::endl;
                    send(client_fd, "Bienvenue sur le chat IRC !\n", 28, 0);
                }
                else
                {
                    char buffer[512];
                    int bytes = recv(clients[i].get_fd_client(), buffer, sizeof(buffer) - 1, 0);
                    if (bytes <= 0)
                    {
                        std::cout << "Client déconnecté : " << clients[i].get_fd_client() << std::endl;
                        close(clients[i].get_fd_client());
                        FD_CLR(clients[i].get_fd_client(), &master_set);
                    }
                    else
                    {
                        buffer[bytes - 1] = '\0';
                        std::cout << "Debut du parsing" << std::endl;
                        std::string ircmsg = buffer;
                        IRC = parseIRCMessage(ircmsg);
                        std::cout << "\n" << std::endl;
                        std::cout << IRC.command << " <--CMD" << std::endl;
                        for (size_t i = 0; i < IRC.params.size(); i++)
                            std::cout << IRC.params[i] << " <--param" << std::endl;
                        std::cout << IRC.prefix << " <--prefix" << std::endl;
                        std::cout << "\n" << std::endl;

                        std::cout << "Fin du parsing" << std::endl;
                        std::cout << "commande du client " << clients[i].get_nickname() << " : " << buffer << std::endl;

                        Command cmd = parse_command(IRC.command.c_str(), clients[i]);

                        switch (cmd) {
                            case CMD_EXIT:
                                send(clients[i].get_fd_client(), "vous vous etes deconecter !", 27, 0);
                                std::cout << "Client déconnecté : " << clients[i].get_fd_client() << std::endl;
                                close(clients[i].get_fd_client());
                                FD_CLR(clients[i].get_fd_client(), &master_set);
                                // supp le client du vector
                                break;

                            case CMD_PING:
                                send(clients[i].get_fd_client(), "PONG\n", 5, 0);
                                break;

                            case CMD_NICK:
                                change_nickname(clients, IRC.params[0], i);
                                break;

                            case CMD_NONICK:
                                send(clients[i].get_fd_client(), "vous devez d'abord changer de nickname !\n", 41, 0);
                                break;

                            case CMD_USER:
                                change_username(clients[i], IRC.params[0]);
                                break;

                            case CMD_NOUSER:
                                send(clients[i].get_fd_client(), "vous devez d'abord changer de username !\n", 41, 0);
                                break;

                            case CMD_JOIN:
                                join_chanel(clients[i], chanel, IRC.params[0]);
                                break;

                            case CMD_PART:
                                part_chanel(clients[i], chanel, IRC.params[0]);
                                break;

                            case CMD_PRIVMSG: // ne marche plus voir pour regler le probleme mais changer l'obj chanel
                                privmsg(clients, IRC.params[0], IRC.params, clients[i]);
                                break;

                           case CMD_KICK:
                                if (check_modo(chanel, IRC.params[0], clients[i].get_username()))
                                {
                                    chanel.del_user(IRC.params[1]);
                                    std::cout << "Client a retirer quelqu'un" << std::endl;
                                }
                                break;

                            case CMD_INVITE:
                                if (check_modo(chanel, IRC.params[0], clients[i].get_username()))
                                {
                                    chanel.add_user(IRC.params[1]);
                                    std::cout << "Client a invité quelqu'un" << std::endl;
                                }
                                break;

                            case CMD_TOPIC:
                                if (check_modo(chanel, IRC.params[0], clients[i].get_username()))
                                {
                                    std::cout << "Client a changer le theme." << std::endl;
                                }
                                break;

                            case CMD_MODE:
                                if (check_modo(chanel, IRC.params[0], clients[i].get_username()))
                                {
                                    std::cout << "Client a ouvert les parametres" << std::endl; 
                                }
                                break;

                            case CMD_UNKNOWN: // a supp mais reste present pour les test d'envoie de message
                                std::cout << "Unknown commande"<<std::endl;
                                // for (int other_fd = 0; other_fd <= max_fd; other_fd++) {
                                //     if (FD_ISSET(other_fd, &master_set) &&
                                //         other_fd != this->fd_server && other_fd != clients[i].get_fd_client())
                                //     {
                                //         send_msg(clients[i].get_nickname(), buffer, other_fd);
                                //     }
                                // }
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