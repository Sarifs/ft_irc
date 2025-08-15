/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoumare <asoumare@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 20:39:24 by asoumare          #+#    #+#             */
/*   Updated: 2025/08/15 22:27:56 by asoumare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

class Client
{
private:
    int id;
    std::string nickname;
    std::string username;
    std::string chanelname;
    bool modo;
public:
    Client();
    ~Client();

    int get_id(void);
    std::string get_username(void);
    std::string get_nickname(void);
    std::string get_chanelname(void);

    void set_id(int string);
    void set_username(std::string string);
    void set_nickname(std::string string);
    void set_chanelname(std::string string);

};


#endif