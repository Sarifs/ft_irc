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

Chanel::Chanel(std::string name, Client user) : name(name), only_invite(false), topic(true), size(-1)
{
    users.push_back(user);
    modo.push_back(user.get_nickname());
    topic_msg = "no topic is set";
}

Chanel::~Chanel()
{
}

// getter

std::string Chanel::get_name(void)
{
    return name;
}

std::vector<Client>& Chanel::get_user(void)
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

size_t Chanel::get_size(void)
{
    return size;
}

bool Chanel::get_invite(void)
{
    return only_invite;
}

std::string Chanel::get_mdp(void)
{
    return mdp;
}

std::string Chanel::get_topic(void)
{
    return topic_msg;
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

void Chanel::del_chanel(void)
{
    std::cout << "Suppression du channel " << name << std::endl;
    name.clear();
    users.clear();
    modo.clear();
}

void Chanel::del_modo(std::string name)
{
    for (std::vector<std::string>::iterator it = modo.begin(); it != modo.end(); )
    {
        if (*it == name)
            it = modo.erase(it);
        else
            ++it;
    }

    std::cout << "Taille des modos après suppression : " << modo.size() << std::endl;

    if (modo.empty() && !users.empty())
    {
        modo.push_back(users[0].get_nickname());
        std::cout << "Nouveau modo : " << users[0].get_nickname() << std::endl;
    }
}

void Chanel::del_user(std::string name)
{
    bool was_modo = false;

    for (size_t i = 0; i < modo.size(); ++i)
    {
        if (modo[i] == name)
        {
            was_modo = true;
            break;
        }
    }

    for (std::vector<Client>::iterator it = users.begin(); it != users.end(); )
    {
        if (it->get_nickname() == name)
            it = users.erase(it);
        else
            ++it;
    }
    
    if (users.empty())
    {
        del_chanel();
        return;
    }

    if (was_modo)
        del_modo(name);
}

// utiles

Chanel* set_chanel(std::vector<Chanel>& chanels, std::string name, bool create, Client &user)
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
        user.set_chanelname(name);
        return NULL;
    }

    return NULL;
}

void Chanel::chanel_only_invite(bool i)
{
    only_invite = i;
}

void Chanel::set_topic(bool i)
{
    topic = i;
}

void Chanel::set_size(std::string c)
{
    size_t i = static_cast<size_t>(atoi(c.c_str()));

    if (i > 0) {
        size = i;
        std::cout << "Limite du channel définie à " << size << " utilisateurs." << std::endl;
    } else {
        std::cout << "Taille invalide: " << c << std::endl;
    }
}

void Chanel::set_mdp(std::string pass)
{
    mdp = pass;
}

void Chanel::set_topic(std::string topic)
{
    topic_msg = topic;
}
