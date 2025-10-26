/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 20:39:24 by asoumare          #+#    #+#             */
/*   Updated: 2025/10/26 17:38:24 by marvin           ###   ########.fr       */
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
    int fd;
    std::string ip;
    std::string nickname;
    std::string username;
    std::vector<std::string> chanelname;
    bool authenticated;
public:
    Client(int fd_client, char *ip);
    ~Client();

    std::string get_ip(void);
    std::string get_username(void);
    std::string get_nickname(void);
    std::vector<std::string> get_chanelname(void);
    int get_fd_client(void);
    void set_username(std::string string);
    void set_nickname(std::string string);
    void set_chanelname(std::string string);

    bool is_authenticated();
    void set_authenticated(bool value);
    
};

void change_nickname(std::vector<Client>& clients, std::string user, int i);

void change_username(Client &clients, std::string user, Client &client);

void join_chanel(Client &client, Chanel *chanel, std::string mdp);

void part_chanel(Client &client, Chanel *chanel, const std::string &name); //, std::vector<Client>& clients);

void privmsg(std::vector<Client> clients, std::vector<std::string> msg, Client client, std::vector<Chanel> chanels, std::string cmd);

bool check_modo(Chanel *chanel, Client client);

void send_msg(Client Client, std::vector<std::string> msg, int fd, std::string cmd);

std::string send_prefix(Client Client);

void cmd_mode(std::string param, Chanel chanel, std::vector<std::string> plus);

void send_in_chanel(std::vector<std::string> msg, Client client, Chanel chanel);

void send_action(Client client, int fd, std::string cmd, std::string dest, std::string msg);

void mode_cmd(std::vector<std::string> cdm, Chanel *chanel);