/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanel.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 18:46:44 by marvin            #+#    #+#             */
/*   Updated: 2025/08/27 18:46:44 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Chanel.hpp"
#include <vector>

Chanel::Chanel(std::string name, std::string user_name) : name(name)
{
    user.push_back(user_name);
    modo.push_back(user_name);
}

Chanel::~Chanel()
{
}

// getter

std::string Chanel::get_name(void)
{
    return name;
}

std::vector<std::string> Chanel::get_user(void)
{
    return user;
}

std::vector<std::string> Chanel::get_conv(void)
{
    return conv;
}

std::vector<std::string> Chanel::get_modo(void)
{
    return modo;
}

// add

void Chanel::add_user(std::string name)
{
    user.push_back(name);
}

void Chanel::add_conv(std::string txt)
{
    conv.push_back(txt);
}

void Chanel::add_modo(std::string name)
{
    modo.push_back(name);
}

//del

void Chanel::del_modo(std::string name)
{
    for (size_t i = 0; i < modo.size(); i++) {
        if(name == modo[i])
            modo.erase(modo.begin() + i);
    }
}

void Chanel::del_user(std::string name)
{
    for (size_t i = 0; i < user.size(); i++) {
        if(name == user[i])
            user.erase(user.begin() + i);
    }
}