/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoumare <asoumare@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 20:47:47 by asoumare          #+#    #+#             */
/*   Updated: 2025/08/15 23:07:07 by asoumare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

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