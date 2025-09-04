/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoumare <asoumare@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 20:39:24 by asoumare          #+#    #+#             */
/*   Updated: 2025/09/04 19:40:45 by asoumare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>

class Chanel;

class Client
{
private:
    int id;
    int fd;
    std::string nickname;
    std::string username;
    std::string chanelname;
public:
    Client(int fd_client);
    ~Client();

    int get_id(void);
    std::string get_username(void);
    std::string get_nickname(void);
    std::string get_chanelname(void);
    int get_fd_client(void);
    void set_id(int string);
    void set_username(std::string string);
    void set_nickname(std::string string);
    void set_chanelname(std::string string);

    
};

void change_nickname(std::vector<Client>& clients, std::string user, int i);

void change_username(Client &clients, std::string user);

void join_chanel(Client &client, Chanel &chanel, const std::string &name);

void part_chanel(Client &client, Chanel &chanel, const std::string &name);

void send_msg(std::string name, char *msg, int fd);

bool check_modo(Chanel chanel, std::string c_name , std::string name);