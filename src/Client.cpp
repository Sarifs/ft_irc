/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoumare <asoumare@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 20:47:47 by asoumare          #+#    #+#             */
/*   Updated: 2025/09/08 17:54:49 by asoumare         ###   ########.fr       */
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


void change_username(Client &clients, std::string user) // a refaire
{
    // std::string user(user);
    user.erase(user.find_last_not_of(" \r\n") + 1);
    
    if (user.empty())
        send(clients.get_fd_client(), "Erreur: pseudo invalide\n", 25, 0);
    else
    {
        clients.set_username(user);
        send(clients.get_fd_client(), "vous avez changer de pseudo !\n", 30, 0);
        std::cout << "Client " << clients.get_fd_client() << " a changé son pseudo en: " << user << std::endl;
    }
}

void join_chanel(Client &client, Chanel *chanel)
{
    std::vector<std::string> users = chanel->get_user();

    for (size_t i = 0; i < users.size(); i++)
    {
        if (users[i] == client.get_username())
        {
            send(client.get_fd_client(), "Vous êtes déjà dans ce channel !\n", 35, 0);
            return;
        }
    }

    chanel->add_user(client.get_username());
    send(client.get_fd_client(), "Vous avez rejoint le channel !\n", 32, 0);
}

void part_chanel(Client &client, Chanel *chanel, const std::string &name, std::vector<Client>& clients)
{
    if (!chanel)
    {
        send(client.get_fd_client(), "Channel introuvable !\n", 23, 0);
        std::cout << "Channel introuvable : " << name << std::endl;
        return ;
    }
    std::vector<std::string> users = chanel->get_user();

    for (size_t i = 0; i < users.size(); i++)
    {
        if (client.get_username() == users[i])
        {
            chanel->del_user(client.get_username());

            for (size_t i = 0; i < clients.size(); i++)
            {
                for (size_t j = 0; j < users.size(); j++)
                {
                    if (clients[i].get_nickname() == users[j] && client.get_nickname() != users[j])
                    send_action(client, clients[i].get_fd_client(), "PART", chanel->get_name(), "NULL");
                }
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
                send_in_chanel(clients, msg, client, chanels[i]);
        }
        return ;
    }
    
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i].get_nickname() == msg[0])
            send_msg(client, msg, clients[i].get_fd_client(), cmd);
    }
    
}

bool check_modo(Chanel *chanel, std::string c_name , std::string name)
{
    if (chanel->get_name() == c_name)
    {
        std::vector<std::string> modo = chanel->get_modo();
        for (size_t j = 0; j < modo.size(); j++)
        {
            if (name == modo[j])
                return true;
        }
    }
    // send ":server 482 <nick> #channel :You're not channel operator"
    return false;
}

void send_in_chanel(std::vector<Client> clients, std::vector<std::string> msg, Client client, Chanel chanel)
{
    std::vector<std::string> user = chanel.get_user();
    for (size_t i = 0; i < user.size(); i++)
    {
        if (client.get_nickname() == user[i])
        {
            for (size_t j = 0; j < clients.size(); j++)
            {
                for (size_t k = 0; k < user.size(); k++)
                {
                    if (user[k] == clients[j].get_nickname() && user[k] != client.get_nickname())
                        send_msg(client, msg, clients[j].get_fd_client(), "PRIVMSG");
                }
            }
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