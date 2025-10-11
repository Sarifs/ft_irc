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
#include "../include/Client.hpp"
#include <cstdlib>

Chanel::Chanel(std::string name, Client &creator)
: name(name), only_invite(false), topic(true), size(static_cast<size_t>(-1)) {
    users.push_back(&creator);
    modo.push_back(creator.get_nickname());
}

Chanel::~Chanel() {}

// getters

const std::string& Chanel::get_name() const { return name; }
const std::vector<Client*>& Chanel::get_user() const { return users; }
const std::vector<std::string>& Chanel::get_conv() const { return conv; }
const std::vector<std::string>& Chanel::get_modo() const { return modo; }
size_t Chanel::get_size() const { return size; }
bool Chanel::get_invite() const { return only_invite; }
const std::string& Chanel::get_mdp() const { return mdp; }

// add

void Chanel::add_user(Client &user) { users.push_back(&user); }
void Chanel::add_conv(const std::string &txt) { conv.push_back(txt); }
void Chanel::add_modo(const std::string &name) { modo.push_back(name); }

// del

void Chanel::del_modo(const std::string &name) {
    for (size_t i = 0; i < modo.size(); ++i)
        if (name == modo[i])
            modo[i].clear();
}

void Chanel::del_user(const std::string &nickname) {
    for (std::vector<Client*>::iterator it = users.begin(); it != users.end(); ) {
        if (*it && (*it)->get_nickname() == nickname) it = users.erase(it);
        else ++it;
    }
}

// utiles

Chanel* set_chanel(std::vector<Chanel>& chanels, const std::string &name, bool create, Client &user) {
    for (size_t i = 0; i < chanels.size(); ++i) {
        if (chanels[i].get_name() == name) return &chanels[i];
    }
    if (create) {
        std::cout << "nouveau chanel cree" << std::endl;
        send(user.get_fd_client(), "vous avez cree un nouvaux chanel !\n", 35, 0);
        chanels.push_back(Chanel(name, user));
        return &chanels.back();
    }
    return NULL;
}

void Chanel::chanel_only_invite(bool i) { only_invite = i; }
void Chanel::set_topic(bool i) { topic = i; }

void Chanel::set_size(const std::string &c) {
    long v = std::strtol(c.c_str(), NULL, 10);
    if (v < 0) {
        size = static_cast<size_t>(-1);
        std::cout << "Limite retiree (illimite)." << std::endl;
        return;
    }
    size_t i = static_cast<size_t>(v);
    if (i > 0) {
        size = i;
        std::cout << "Limite du channel definie a " << size << " utilisateurs." << std::endl;
    } else {
        std::cout << "Taille invalide: " << c << std::endl;
    }
}

void Chanel::set_mdp(const std::string &pass) { mdp = pass; }
