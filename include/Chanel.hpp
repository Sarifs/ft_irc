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

#pragma once 

#include "Client.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

class Chanel
{
private:
    std::string name;
    std::vector<Client> users;
    std::vector<std::string> conv;
    std::vector<std::string> modo;
    std::string topic_msg;
    std::string mdp;
    bool only_invite;
    bool topic;
    int size;

public:
    Chanel(std::string name, Client user);
    ~Chanel();

    //getter

    std::string get_name(void);
    std::vector<Client>& get_user();
    std::vector<std::string> get_conv();
    std::vector<std::string> get_modo();
    std::string get_mdp();
    std::string get_topic();
    size_t get_size();
    bool get_invite();

    //add

    void add_user(Client client);
    void add_modo(std::string name);
    void add_conv(std::string txt);

    //del

    void del_chanel(void);
    void del_user(std::string name);
    void del_modo(std::string name);

    // setter
    void chanel_only_invite(bool i);
    void set_topic(bool i);
    void set_size(std::string c);
    void set_mdp(std::string pass);
    void set_topic(std::string topic);

};

Chanel* set_chanel(std::vector<Chanel>& chanels, std::string name, bool create, Client &user);
