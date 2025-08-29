/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 20:47:47 by asoumare          #+#    #+#             */
/*   Updated: 2025/08/29 18:50:34 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client() : id(0), nickname("nickname"), username("username"), chanelname("void") {}

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

void Client::set_nickname(std::string string)
{
    nickname = string;
}

void Client::set_username(std::string string)
{
    username = string;
}

void Client::set_chanelname(std::string string)
{
    chanelname = string;
}

// ft utile

void send_msg(std::string name_s, char *msg, int fd)
{
    std::cout << "DEBUG (server): [" << name_s << "]" << std::endl;

    int sent = send(fd, name_s.c_str(), name_s.size(), 0);
    if (sent == -1) std::cerr <<"send name" <<std::endl;

    sent = send(fd, ": ", 2, 0);
    if (sent == -1) std::cerr << "send sep" << std::endl;

    sent = send(fd, msg, strlen(msg), 0);
    if (sent == -1) std::cerr << "send msg" << std::endl;

    sent = send(fd, "\r\n", 2, 0);
    if (sent == -1) std::cerr << "send newline" << std::endl;
}
