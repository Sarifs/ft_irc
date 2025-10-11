/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chanel.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 17:01:52 by marvin            #+#    #+#             */
/*   Updated: 2025/08/27 17:01:52 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANEL_HPP
#define CHANEL_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sys/socket.h>

class Client;

class Chanel {
public:
    Chanel(std::string name, Client &creator);
    ~Chanel();

    // getters
    const std::string&              get_name() const;
    const std::vector<Client*>&     get_user() const;
    const std::vector<std::string>& get_conv() const;
    const std::vector<std::string>& get_modo() const;
    size_t                          get_size() const;
    bool                            get_invite() const;
    const std::string&              get_mdp() const;

    // add
    void add_user(Client &user);
    void add_conv(const std::string &txt);
    void add_modo(const std::string &name);

    // del
    void del_modo(const std::string &name);
    void del_user(const std::string &nickname);

    // modes
    void chanel_only_invite(bool i);
    void set_topic(bool i);
    void set_size(const std::string &c);
    void set_mdp(const std::string &pass);

private:
    std::string              name;
    std::vector<Client*>     users;    // non owning pointers
    std::vector<std::string> conv;
    std::vector<std::string> modo;     // nicknames
    bool                     only_invite;
    bool                     topic;
    size_t                   size;     // -1 = illimité
    std::string              mdp;
};

// util hors classe
Chanel* set_chanel(std::vector<Chanel>& chanels, const std::string &name, bool create, Client &user);

#endif
