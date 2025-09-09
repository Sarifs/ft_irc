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
#include <vector>

class Chanel
{
private:
    std::string name;
    std::vector<Client> users;
    std::vector<std::string> conv;
    std::vector<std::string> modo;

public:
    Chanel(std::string name, Client user);
    ~Chanel();

    //getter

    std::string get_name(void);
    std::vector<Client> get_user();
    std::vector<std::string> get_conv();
    std::vector<std::string> get_modo();

    //add

    void add_user(Client client);
    void add_modo(std::string name);
    void add_conv(std::string txt);

    //del

    void del_user(std::string name);
    void del_modo(std::string name);
};

Chanel* set_chanel(std::vector<Chanel>& chanels, std::string name, bool create, Client user);
