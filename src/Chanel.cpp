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

Chanel::Chanel(std::string name, Client user) : name(name)
{
    users.push_back(user);
    modo.push_back(user.get_nickname());
}

Chanel::~Chanel()
{
}

// getter

std::string Chanel::get_name(void)
{
    return name;
}

std::vector<Client> Chanel::get_user(void)
{
    return users;
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

void Chanel::add_user(Client name)
{
    users.push_back(name);
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
            modo[i] = "\0";
    }
}

void Chanel::del_user(std::string name)
{
    for (std::vector<Client>::iterator i = users.begin(); i != users.end(); )
    {
        if (i->get_nickname() == name)
            i = users.erase(i);
        else
            ++i;
    }

    for (size_t i = 0; i < users.size(); i++)
        std::cout << users[i].get_nickname() << std::endl;
}




// utiles

Chanel* set_chanel(std::vector<Chanel>& chanels, std::string name, bool create, Client user)
{
    for (size_t i = 0; i < chanels.size(); i++)
    {
        if (chanels[i].get_name() == name) {
            return &chanels[i];
        }
    }

    if (create)
    {
        std::cout << "nouveau chanel cree" << std::endl;
        send(user.get_fd_client(), "vous avez cree un nouvaux chanel !\n", 35, 0);
        chanels.push_back(Chanel(name, user));
        return &chanels.back();
    }

    return NULL;
}
