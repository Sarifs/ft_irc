/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoumare <asoumare@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 20:47:47 by asoumare          #+#    #+#             */
/*   Updated: 2025/09/04 19:40:49 by asoumare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include "../include/Chanel.hpp"


Client::Client(int fd_client): fd(fd_client){
}
//Client::Client() : id(0), nickname("nickname"), username("username"), chanelname("void") {}

Client::~Client() {}

 // getter

int Client::get_id(void)
{
    return id;
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

void Client::set_id(int string)
{
    id = string;
}

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
// ft utile

void send_msg(std::string name_s, char *msg, int fd)
{

    send(fd, name_s.c_str(), name_s.size(), 0);

    send(fd, ": ", 2, 0);

    send(fd, msg, strlen(msg), 0);

    send(fd, "\r\n", 2, 0);
}

void change_nickname(std::vector<Client>& clients, std::string nick, int i)
{
    nick.erase(nick.find_last_not_of(" \r\n") + 1);

    if (nick.empty())
    {
        send(clients[i].get_fd_client(), "Erreur: pseudo invalide\n", 25, 0);
        return;
    }

    if (!clients[i].get_nickname().empty())
    {
        send(clients[i].get_fd_client(), "vous ne pouvez pas changer de nickname !\n", 41, 0);
        return;
    }

    for (size_t j = 0; j < clients.size(); j++)
    {
        if (j != (size_t)i && clients[j].get_nickname() == nick)
        {
            send(clients[i].get_fd_client(), "ce nickname est déjà utilisé !\n", 33, 0);
            return;
        }
    }

    clients[i].set_nickname(nick);
    send(clients[i].get_fd_client(), "vous avez changé de pseudo !\n", 30, 0);
    std::cout << "Client " << clients[i].get_fd_client() 
              << " a changé son pseudo en: " << nick << std::endl;
}


void change_username(Client &clients, std::string user)
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

void join_chanel(Client &client, Chanel &chanel, const std::string &name)
{
    if (chanel.get_name() == name)
    {
        std::vector<std::string> users = chanel.get_user();

        for (size_t i = 0; i < users.size(); i++)
        {
            if (users[i] == client.get_username())
            {
                send(client.get_fd_client(), "Vous êtes déjà dans ce channel !\n", 35, 0);
                return;
            }
        }

        chanel.add_user(client.get_username());
        send(client.get_fd_client(), "Vous avez rejoint le channel !\n", 33, 0);
    }
    else
        send(client.get_fd_client(), "Channel introuvable !\n", 23, 0);
}

void part_chanel(Client &client, Chanel &chanel, const std::string &name)
{
    if (chanel.get_name() == name)
    {
        std::vector<std::string> users = chanel.get_user();

        for (size_t i = 0; i < users.size(); i++)
        {
            if (client.get_username() == users[i])
            {
                chanel.del_user(client.get_username());
                send(client.get_fd_client(),
                     "Vous avez quitté le channel.\n", 31, 0);
                std::cout << "Client " << client.get_username()
                          << " a quitté le channel " << name << std::endl;
                return;
            }
        }

        send(client.get_fd_client(), "Vous n'êtes pas dans ce channel !\n", 35, 0);
        std::cout << "Client " << client.get_username() << " a essayé de quitter " << name << " sans y être.\n";
    }
    else
    {
        send(client.get_fd_client(), "Channel introuvable !\n", 23, 0);
        std::cout << "Channel introuvable : " << name << std::endl;
    }
}

bool check_modo(Chanel chanel, std::string c_name , std::string name)
{
    if (chanel.get_name() == c_name)
    {
        std::vector<std::string> modo = chanel.get_modo();
        for (size_t j = 0; j < modo.size(); j++)
        {
            if (name == modo[j])
                return true;
        }
    }
    return false;
}
