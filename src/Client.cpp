/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 20:47:47 by asoumare          #+#    #+#             */
/*   Updated: 2025/08/31 15:12:01 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"


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
    std::cout << "DEBUG (server): [" << name_s << "]" << std::endl;

    send(fd, name_s.c_str(), name_s.size(), 0);

    send(fd, ": ", 2, 0);

    send(fd, msg, strlen(msg), 0);

    send(fd, "\r\n", 2, 0);
}
