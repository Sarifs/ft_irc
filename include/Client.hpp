/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdiomand <mdiomand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 20:39:24 by asoumare          #+#    #+#             */
/*   Updated: 2025/10/11 15:20:04 by mdiomand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
public:
    Client(int fd_client, const char *ip);
    Client();
    ~Client(); // ne ferme PAS le fd (fermeture centralisée dans le serveur)

    // getters
    int         get_fd_client() const;
    std::string get_ip() const;
    std::string get_nickname() const;
    std::string get_username() const;
    std::string get_chanelname() const;

    // setters
    void set_nickname(const std::string &nickname);
    void set_username(const std::string &username);
    void set_chanelname(const std::string &chanel_name);

    // auth
    bool is_authenticated() const;
    void set_authenticated(bool value);

    // util
    void mark_fd_closed(); // met fd = -1 (utile après close())

private:
    int         fd;
    std::string ip;
    std::string nickname;
    std::string username;
    std::string chanelname;
    bool        authenticated;
};

#endif
