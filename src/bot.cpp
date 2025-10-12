/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idioumas <idioumas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 16:34:56 by idioumas          #+#    #+#             */
/*   Updated: 2025/10/12 21:22:30 by idioumas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/bot.hpp"
#include <sys/socket.h>
#include <netinet/in.h>

bot::bot()
{
    
}

bot::~bot()
{
    
}


bot::bot(const char * adr, const unsigned int port)
{
    
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
}