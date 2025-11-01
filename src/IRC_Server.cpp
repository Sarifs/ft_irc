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
#include <csignal>

enum Command {
    CMD_EXIT,
    CMD_PING,
    CMD_PASS,
    CMD_NOPASS,
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
    if (!strcmp(buffer, "QUIT")) return CMD_EXIT;
    if (!strcmp(buffer, "PING")) return CMD_PING;
    if (!strcmp(buffer, "PASS")) return CMD_PASS;
    if (client.is_authenticated() == false) return CMD_NOPASS;
    if (!strcmp(buffer, "NICK")) return CMD_NICK;
    if (!strcmp(client.get_nickname().c_str(), "")) return CMD_NONICK;
    if (!strcmp(buffer, "USER")) return CMD_USER;
    if (!strcmp(client.get_username().c_str(), "")) return CMD_NOUSER;
    if (!strcmp(buffer, "JOIN")) return CMD_JOIN;
    if (!strcmp(buffer, "PART")) return CMD_PART;
    if (!strcmp(buffer, "PRIVMSG")) return CMD_PRIVMSG;
    if (!strcmp(buffer, "KICK")) return CMD_KICK;
    if (!strcmp(buffer, "INVITE")) return CMD_INVITE;
    if (!strcmp(buffer, "TOPIC")) return CMD_TOPIC;
    if (!strcmp(buffer, "MODE")) return CMD_MODE;
    return CMD_UNKNOWN;
}

IRC_Serveur::IRC_Serveur(int port, std::string pass) : passwold(pass)
{
    this->fd_server = socket(AF_INET,SOCK_STREAM,0);
    if (this->fd_server < 0)
    {
        std::cerr << "Error socket creation" << std::endl;
        exit(1);
    }
    int opt = 1;
    setsockopt(this->fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(port);

    if (bind(this->fd_server, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error binding association failed" << std::endl;
        close(fd_server);
        exit(1);
    }
    if (listen(this->fd_server, SOMAXCONN) < 0) {
        std::cerr << "Error listen" << std::endl;
        close(this->fd_server);
        exit(1);
    }
    std::cout << "Serveur IRC en écoute sur le port " << port << "..." << std::endl;
}

std::string IRC_Serveur::get_password()
{
    return passwold;
}

void ft_bzero(IRCMessage &irc)
{
    irc.command = "";
    irc.prefix = "";
    irc.params.assign(4, "");
}

void leave_irc(int sig)
{
    (void)sig;
    std::cout << "\nLeave irc" << std::endl;
}

void IRC_Serveur::run()
{
    IRCMessage IRC;
    std::vector<Client> clients;
    std::vector<Chanel> chanels;

    fd_set master_set, read_set;
    signal(SIGINT,leave_irc);
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    int max_fd = this->fd_server;
    char ip[INET_ADDRSTRLEN];
    memset(&address, 0, sizeof(address));
    inet_ntop(AF_INET, &(address.sin_addr), ip, INET_ADDRSTRLEN);
    Client srvr(this->fd_server, ip);
    clients.push_back(srvr);
    FD_ZERO(&master_set);
    FD_SET(this->fd_server, &master_set);
    
    while (1)
    {
        read_set = master_set;
        if (select(max_fd + 1, &read_set, NULL, NULL, NULL) < 0)
            break;

        for (size_t i = 0; i < clients.size(); i++)
        {
            if (FD_ISSET(clients[i].get_fd_client(), &read_set))
            {
                if (clients[i].get_fd_client() == this->fd_server)
                {
                    int client_fd = accept(this->fd_server, (struct sockaddr*)&address, &addrlen);
                    if (client_fd >= 0)
                    {                       
                        char ip[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &(address.sin_addr), ip, INET_ADDRSTRLEN);
 
                        Client new_client(client_fd, ip);
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
                        for (size_t p = 0; p < clients[i].get_chanelname().size(); p++)
                        {
                            std::string chanel_name = clients[i].get_chanelname()[p];
                            Chanel *chanel_tmp = set_chanel(chanels, chanel_name, false, clients[i]);
                            part_chanel(clients[i], chanel_tmp, chanel_name);
                        }
                        std::cout << "Client déconnecté brutalement : " << clients[i].get_nickname() << std::endl;
                        close(clients[i].get_fd_client());
                        FD_CLR(clients[i].get_fd_client(), &master_set);
                        clients.erase(clients.begin() + i);
                        continue;
                    }
                    else
                    {
                        buffer[bytes - 1] = '\0';
                        std::string ircmsg = buffer;
                        ft_bzero(IRC);
                        IRC = parseIRCMessage(ircmsg);
                        if (IRC.prefix != "") continue;
                        std::cout << "commande du client " << clients[i].get_nickname() << " : " << buffer << std::endl;
                        Command cmd = parse_command(IRC.command.c_str(), clients[i]);

                        switch (cmd)
                        {
                            case CMD_EXIT:
                                for (size_t p = 0; p < clients[i].get_chanelname().size(); p++)
                                {
                                    std::string chanel_name = clients[i].get_chanelname()[p];
                                    std::cout << chanel_name << std::endl;
                                    Chanel *chanel_tmp = set_chanel(chanels, chanel_name, false, clients[i]);
                                    part_chanel(clients[i], chanel_tmp, chanel_name);
                                }
                                send(clients[i].get_fd_client(), "vous vous etes deconecter !\n", 28, 0);
                                std::cout << "Client déconnecté : " << clients[i].get_fd_client() << std::endl;
                                close(clients[i].get_fd_client());
                                FD_CLR(clients[i].get_fd_client(), &master_set);
                                clients.erase(clients.begin() + i);
                                break;

                            case CMD_PING:
                                send(clients[i].get_fd_client(), ":server PONG\n", 13, 0);
                                break;

                            case CMD_PASS:
                            {
                                if (clients[i].is_authenticated() == true) break;
                                if (IRC.params[0] == this->get_password())
                                {
                                    clients[i].set_authenticated(true);
                                    send(clients[i].get_fd_client(), "Mot de passe correct, vous pouvez continuer.\n", 45, 0);
                                }
                                else
                                    send(clients[i].get_fd_client(), "Mot de passe incorrect, veillez reessayer.\n", 43, 0);
                                break;
                            }

                            case CMD_NOPASS:
                                send(clients[i].get_fd_client(), "Vous devez d'abord entrer le mot de passe avec PASS.\n", 53, 0);
                                break;

                            case CMD_NICK:
                                change_nickname(clients, IRC.params[0], i);
                                break;

                            case CMD_NONICK:
                                send(clients[i].get_fd_client(), "vous devez d'abord changer de nickname !\n", 41, 0);
                                break;

                            case CMD_USER:
                                if (IRC.params[3] == "")
                                {
                                    send(clients[i].get_fd_client(), "USER <username> <hostname> <servername> :<realname>\n", 52, 0);
                                    break;
                                }
                                change_username(clients[i], IRC.params[0], clients[i]);
                                break;

                            case CMD_NOUSER:
                                send(clients[i].get_fd_client(), "vous devez d'abord changer de username !\n", 41, 0);
                                break;

                            case CMD_JOIN:
                            {
                                for (size_t p = 0; p < IRC.params.size(); p++)
                                {
                                    if (IRC.params[p][0] != '#' || !IRC.params[p][1])
                                    {
                                        std::string reply = ":server 407 " + clients[i].get_nickname() + " " + IRC.params[p] + " :chanel started by '#'\r\n";
                                        send(clients[i].get_fd_client(), reply.c_str(), reply.size(), 0);
                                        continue;
                                    }
                                    Chanel *chanel_tmp = set_chanel(chanels, IRC.params[p], true, clients[i]);
                                    if (!chanel_tmp)
                                        continue;
                                    join_chanel(clients[i], chanel_tmp, IRC.params[1]);
                                }
                                break;
                            }

                            case CMD_PART:
                            {
                                for (size_t p = 0; p < IRC.params.size(); p++)
                                {
                                    Chanel *chanel_tmp = set_chanel(chanels, IRC.params[p], false, clients[i]);
                                    if (!chanel_tmp)
                                    {
                                        std::string reply = ":server 403 " + clients[i].get_nickname() + " " + IRC.params[p] + " :No such channel\r\n";
                                        send(clients[i].get_fd_client(), reply.c_str(), reply.size(), 0);
                                        break;
                                    }
                                    part_chanel(clients[i], chanel_tmp, IRC.params[p]);
                                }
                                break;
                            }

                            case CMD_PRIVMSG:
                                privmsg(clients, IRC.params, clients[i], chanels, IRC.command);
                                break;

                            case CMD_KICK:
                            {
                                Chanel *chanel_tmp = set_chanel(chanels, IRC.params[0], false, clients[i]);
                                if (!chanel_tmp)
                                {
                                    std::string reply = ":server 403 " + clients[i].get_nickname() + " " + IRC.params[0] + " :No such channel\r\n";
                                    send(clients[i].get_fd_client(), reply.c_str(), reply.size(), 0);
                                    break;
                                }

                                if (!check_modo(chanel_tmp, clients[i]))
                                    break;

                                for (size_t p = 1; p < IRC.params.size(); ++p)
                                {
                                    std::cout << "kick : " << IRC.params[p] << std::endl;
                                    chanel_tmp->del_user(IRC.params[p]);
                                }
                                std::cout << "Client a retiré quelqu'un" << std::endl;
                                break;
                            }

                            case CMD_INVITE:
                            {
                                Chanel *chanel_tmp = set_chanel(chanels, IRC.params[0], false, clients[i]);
                                if (!chanel_tmp)
                                {
                                    std::string reply = ":server 403 " + clients[i].get_nickname() + " " + IRC.params[0] + " :No such channel\r\n";
                                    send(clients[i].get_fd_client(), reply.c_str(), reply.size(), 0);
                                    break;
                                }

                                if (!check_modo(chanel_tmp, clients[i]))
                                    break;
                                for (size_t j = 0; j < clients.size(); ++j)
                                {
                                    for (size_t p = 1; p < IRC.params.size(); ++p)
                                    {
                                        if (clients[j].get_nickname() == IRC.params[p])
                                        {
                                            chanel_tmp->add_user(clients[j]);
                                            std::string msg = ":" + clients[i].get_nickname() + " INVITE " + clients[j].get_nickname() + " " + IRC.params[0] + "\r\n";
                                            send(clients[j].get_fd_client(), msg.c_str(), msg.size(), 0);
                                        }
                                    }
                                }

                                std::cout << "Client a invité quelqu'un" << std::endl;
                                break;
                            }

                            case CMD_TOPIC:
                            {
                                Chanel *chanel_tmp = set_chanel(chanels, IRC.params[0], false, clients[i]);
                                if (!chanel_tmp)
                                {
                                    std::string reply = ":server 403 " + clients[i].get_nickname() + " " + IRC.params[0] + " :No such channel\r\n";
                                    send(clients[i].get_fd_client(), reply.c_str(), reply.size(), 0);
                                    break;
                                }
                                std::vector<Client> users = chanel_tmp->get_user();
                                if (check_modo(chanel_tmp, clients[i]))
                                {
                                    if (IRC.params[1] == "")
                                    {
                                        std::string txt = "TOPIC : " + chanel_tmp->get_topic() + "\n";
                                        send(clients[i].get_fd_client(), txt.c_str(), txt.size(), 0);
                                    }
                                    else
                                    {
                                        std::cout << "Client a changer le theme." << std::endl;
                                        chanel_tmp->set_topic(IRC.params[1]);
                                        std::string txt = ":" + clients[i].get_nickname() + " TOPIC " + chanel_tmp->get_name() + " : " + IRC.params[1] + "\n";
                                        for (size_t i = 0; i < users.size(); i++)
                                            send(users[i].get_fd_client(), txt.c_str(), txt.size(), 0);
                                    }
                                }
                                break;
                            }

                            case CMD_MODE:
                            {
                                Chanel *chanel_tmp = set_chanel(chanels, IRC.params[0], false, clients[i]);
                                if (!chanel_tmp)
                                {
                                    std::string reply = ":server 403 " + clients[i].get_nickname() + " "
                                                    + IRC.params[0] + " :No such channel\r\n";
                                    send(clients[i].get_fd_client(), reply.c_str(), reply.size(), 0);
                                    break;
                                }
                                if (IRC.params.size() == 1)
                                    break;
                                if (check_modo(chanel_tmp, clients[i]))
                                {
                                    mode_cmd(IRC.params, chanel_tmp);
                                    std::cout << "Client a ouvert les parametres" << std::endl;
                                }
                                break;
                            }

                            case CMD_UNKNOWN:
                                std::cout << "Unknown commande"<<std::endl;
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
    std::cout << "destruction de tout" <<std::endl;   
}