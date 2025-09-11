/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 20:47:47 by asoumare          #+#    #+#             */
/*   Updated: 2025/09/11 17:38:21 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include "../include/Chanel.hpp"


Client::Client(int fd_client, char *ip): fd(fd_client), authenticated(false) {
    this->ip = std::string(ip);
}
//Client::Client() : id(0), nickname("nickname"), username("username"), chanelname("void") {}

Client::~Client() {}

 // getter

std::string Client::get_ip(void)
{
    return ip;
}

std::string Client::get_nickname(void)
{
    return nickname;
}

std::string Client::get_username(void)
{
    return username;
}

std::string Client::get_chanelname(void)
{
    return chanelname;
}

 // setter

void Client::set_nickname(std::string nickname)
{
    this->nickname = nickname;
}

void Client::set_username(std::string username)
{
    this->username = username;
}

void Client::set_chanelname(std::string chanel_name)
{
    this->chanelname = chanel_name;
}


int Client::get_fd_client(void)
{
    return this->fd;
}

// authenticated

bool Client::is_authenticated()
{
    return authenticated;
}

void Client::set_authenticated(bool value)
{
    authenticated = value;
}


// ft utile

void send_prefix(Client client, int fd)
{
    send(fd, ":", 1, 0);
        
    send(fd, client.get_nickname().c_str(), client.get_nickname().size(), 0);
    
    send(fd, "!", 1, 0);
    
    send(fd, client.get_username().c_str(), client.get_username().size(), 0);
    
    send(fd, "@", 1, 0);

    send(fd, client.get_ip().c_str(), client.get_ip().size() , 0);

    send(fd, " ", 1, 0);
}

void send_msg(Client client, std::vector<std::string> msg, int fd, std::string cmd)
{
    send_prefix(client, fd);

    send(fd, cmd.c_str(), cmd.size(), 0);

    send(fd, " ", 1, 0);

    send(fd, msg[0].c_str(), msg[0].size(), 0);

    send(fd, " : ", 3, 0);

    for (size_t i = 1; i < msg.size(); i++)
    {
        send(fd, msg[i].c_str(), strlen(msg[i].c_str()), 0);
        send(fd, " ", 1, 0);
    }

    send(fd, "\r\n", 2, 0);
}

void change_nickname(std::vector<Client>& clients, std::string nick, int i)
{
    nick.erase(nick.find_last_not_of(" \r\n") + 1);

    if (nick.empty())
    {
        send(clients[i].get_fd_client(), "Erreur: pseudo invalide (vide)\n", 25, 0);
        return;
    }

    if (!clients[i].get_nickname().empty())
    {
        send(clients[i].get_fd_client(), "vous ne pouvez pas changer de nickname !\n", 41, 0); // pas obliger
        return;
    }

    for (size_t j = 0; j < clients.size(); j++)
    {
        if (j != (size_t)i && clients[j].get_nickname() == nick)
        {
            send(clients[i].get_fd_client(), ":server 433 * ", 14, 0);
            send(clients[i].get_fd_client(), nick.c_str(), nick.size(), 0);
            send(clients[i].get_fd_client(), " :Nickname is already in use\n", 29, 0);
            return;
        }
    }

    clients[i].set_nickname(nick);
    send(clients[i].get_fd_client(), "vous avez changé de pseudo !\n", 30, 0);
    std::cout << "Client " << clients[i].get_fd_client() 
              << " a changé son pseudo en: " << nick << std::endl;
}


void change_username(Client &clients, std::string user, Client &client) // ajouter le -> <hostname> <servername> :<realname>
{
    user.erase(user.find_last_not_of(" \r\n") + 1);
    
    if (user.empty())
        send(clients.get_fd_client(), "Erreur: pseudo invalide\n", 25, 0);
    else
    {
        clients.set_username(user);
        send(clients.get_fd_client(), ":server 001 ", 12, 0);
        send(clients.get_fd_client(), user.c_str(), user.size(), 0);
        send(clients.get_fd_client(), " :Welcome to the IRC server ", 28, 0);
        send_prefix(client, clients.get_fd_client());
        send(clients.get_fd_client(), "\n", 1, 0);
        std::cout << "Client " << clients.get_fd_client() << " a changé son pseudo en: " << user << std::endl;
    }
}

void join_chanel(Client &client, Chanel *chanel, std::string mdp) // ajoute les bool et le MDP
{
    std::vector<Client> users = chanel->get_user();

    for (size_t i = 0; i < users.size(); i++)
    {
        if (users[i].get_nickname() == client.get_username())
        {
            send(client.get_fd_client(), "Vous êtes déjà dans ce channel !\n", 35, 0);
            return;
        }
    }
    if (chanel->get_invite() == true)
        send(client.get_fd_client(), "chanel sur invitation uniqument !\n", 34, 0);
    else if (chanel->get_size() > 0 && chanel->get_user().size() >= chanel->get_size())
        send(client.get_fd_client(), "le chanel est plein !\n", 22, 0);
    else if (chanel->get_mdp() != "" && chanel->get_mdp() != mdp)
        send(client.get_fd_client(), "le mot de pass est incorect !\n", 30, 0);
    else
    {
        for (size_t i = 0; i < users.size(); i++)
        {
            send(users[i].get_fd_client(), ":", 1, 0);
            send(users[i].get_fd_client(), client.get_nickname().c_str(), client.get_nickname().size(), 0);
            send(users[i].get_fd_client(), " JOIN :#", 8, 0);
            send(users[i].get_fd_client(), chanel->get_name().c_str(), chanel->get_name().size(), 0);
        }
        chanel->add_user(client);
        send(client.get_fd_client(), "Vous avez rejoint le channel !\n", 32, 0);
    }
}

void part_chanel(Client &client, Chanel *chanel, const std::string &name)
{
    if (!chanel)
    {
        send(client.get_fd_client(), "Channel introuvable !\n", 23, 0);
        std::cout << "Channel introuvable : " << name << std::endl;
        return ;
    }
    std::vector<Client> users = chanel->get_user();

    for (size_t i = 0; i < users.size(); i++)
    {
        if (client.get_username() == users[i].get_username())
        {
            chanel->del_user(client.get_username());
                for (size_t i = 0; i < users.size(); i++)
                {
                    send(users[i].get_fd_client(), ":", 1, 0);
                    send(users[i].get_fd_client(), client.get_nickname().c_str(), client.get_nickname().size(), 0);
                    send(users[i].get_fd_client(), " PART #", 7, 0);
                    send(users[i].get_fd_client(), chanel->get_name().c_str(), chanel->get_name().size(), 0);
                }
            send(client.get_fd_client(), "Vous avez quitté le channel.\n", 31, 0);
            std::cout << "Client " << client.get_username() << " a quitté le channel " << name << std::endl;
            return;
        }
    }

    send(client.get_fd_client(), "Vous n'êtes pas dans ce channel !\n", 35, 0);
    std::cout << "Client " << client.get_username() << " a essayé de quitter " << name << " sans y être.\n";
}

void privmsg(std::vector<Client> clients, std::vector<std::string> msg, Client client, std::vector<Chanel> chanels, std::string cmd)
{
    if (msg[0][0] == '#')
    {
        const char *name = msg[0].c_str();
        for (size_t i = 0; i < chanels.size(); i++)
        {
            if (!strcmp(&name[1], chanels[i].get_name().c_str()))
            {
                send_in_chanel(msg, client, chanels[i]);
                return;
            }
        }
    }

    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i].get_nickname() == msg[0])
        {
            send_msg(client, msg, clients[i].get_fd_client(), cmd);
            return;
        }
    }
    send(client.get_fd_client(), ":server 401 ", 12, 0);
    send(client.get_fd_client(), client.get_nickname().c_str(), client.get_nickname().size(), 0);
    send(client.get_fd_client(), " ", 1, 0);
    send(client.get_fd_client(), msg[0].c_str(), msg[0].size(), 0);
    send(client.get_fd_client(), " :No such nick/channel\n", 23, 0);
}

bool check_modo(Chanel *chanel, Client client)
{
    std::vector<std::string> modo = chanel->get_modo();
    for (size_t j = 0; j < modo.size(); j++)
    {
        if (client.get_nickname() == modo[j])
            return true;
    }

    // Construire le message IRC 482 complet
    std::string reply = ":server 482 " + client.get_nickname() + " #" + chanel->get_name() + " :You're not channel operator\r\n";
    send(client.get_fd_client(), reply.c_str(), reply.size(), 0);

    return false;
}


void send_in_chanel(std::vector<std::string> msg, Client client, Chanel chanel)
{
    std::vector<Client> user = chanel.get_user();
    for (size_t i = 0; i < user.size(); i++)
    {
        if (client.get_nickname() == user[i].get_username())
        {
            for (size_t i = 0; i < user.size(); i++)
                send_msg(client, msg, user[i].get_fd_client(), "PRIVMSG");
            return ;
        }
    }
    send(client.get_fd_client(), "Vous n'êtes pas dans ce channel !\n", 35, 0);
}

void send_action(Client client, int fd, std::string cmd, std::string dest, std::string msg)
{
    send_prefix(client, fd);

    send(fd, cmd.c_str(), cmd.size(), 0);

    send(fd, " ", 1, 0);

    send(fd, dest.c_str(), dest.size(), 0);

    if (msg != "NULL")
    {
        send(fd, " : ", 3, 0);
        send(fd, msg.c_str(), strlen(msg.c_str()), 0);
    }

    send(fd, "\r\n", 2, 0);
}

void mode_cmd(std::vector<std::string> cdm, Chanel *chanel)
{
    if (cdm[1][1] == 'i' && cdm[1][2] == '\0')
    {
        std::cout << "mode i" << std::endl;
        if (cdm[1][0] == '+')
            chanel->chanel_only_invite(true);
        if (cdm[1][0] == '-')
            chanel->chanel_only_invite(false);
    }
    if (cdm[1][1] == 't' && cdm[1][2] == '\0')
    {
        std::cout << "mode t" << std::endl;
        if (cdm[1][0] == '+')
            chanel->set_topic(true);
        if (cdm[1][0] == '-')
            chanel->set_topic(false);
    }
    if (cdm[1][1] == 'k' && cdm[1][2] == '\0')
    {
        std::cout << "mode k" << std::endl;
        if (cdm[1][0] == '+')
            chanel->set_mdp(cdm[2]);
        if (cdm[1][0] == '-')
            chanel->set_mdp("");
    }
    if (cdm[1][1] == 'o' && cdm[1][2] == '\0')
    {
        std::cout << "mode o" << std::endl;
        if (cdm[1][0] == '+')
            chanel->add_modo(cdm[2]);
        if (cdm[1][0] == '-')
            chanel->del_modo(cdm[2]);
    }
    if (cdm[1][1] == 'l' && cdm[1][2] == '\0')
    {
        std::cout << "mode l" << std::endl;
        if (cdm[1][0] == '+')
            chanel->set_size(cdm[2]);
        if (cdm[1][0] == '-')
            chanel->set_size("-1");
    }
}