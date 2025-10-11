/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdiomand <mdiomand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 20:47:47 by asoumare          #+#    #+#             */
/*   Updated: 2025/10/11 15:19:25 by mdiomand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client(int fd_client, const char *ip_cstr)
: fd(fd_client), ip(ip_cstr ? ip_cstr : ""), authenticated(false) {}

Client::Client() : fd(-1), authenticated(false) {}

Client::~Client() {
    // Fermeture centralisée par le serveur -> ne rien fermer ici
}

int Client::get_fd_client() const { return fd; }
std::string Client::get_ip() const { return ip; }
std::string Client::get_nickname() const { return nickname; }
std::string Client::get_username() const { return username; }
std::string Client::get_chanelname() const { return chanelname; }

void Client::set_nickname(const std::string &v) { nickname = v; }
void Client::set_username(const std::string &v) { username = v; }
void Client::set_chanelname(const std::string &v) { chanelname = v; }

bool Client::is_authenticated() const { return authenticated; }
void Client::set_authenticated(bool v) { authenticated = v; }

void Client::mark_fd_closed() { /* utile si tu veux marquer l'état */ }
