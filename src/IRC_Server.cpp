/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 19:45:14 by marvin            #+#    #+#             */
/*   Updated: 2025/08/28 19:45:14 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRC_Serveur.hpp"
#include "../include/Chanel.hpp"
#include "../include/Parsing.hpp"
#include <cstring>

// pointeur global (simple) pour le handler
static IRC_Serveur* g_server_instance = NULL;

static void leave_irc(int sig) {
    (void)sig;
    if (g_server_instance) g_server_instance->stop();
}

// ==== Helpers locaux (protos) ====
static std::string send_prefix(const Client& client);
static void send_msg(const Client& client, const std::vector<std::string>& msg, int fd, const std::string& cmd);
static void change_nickname(std::vector<Client>& clients, const std::string& nick, size_t idx);
static void change_username(Client &target, const std::string &user, const Client &self);
static void join_chanel(Client &client, Chanel *chanel, const std::string &mdp);
static void part_chanel(Client &client, Chanel *chanel, const std::string &name);
static void send_in_chanel(const std::vector<std::string>& msg, const Client& client, Chanel& chanel);
static void privmsg(const std::vector<Client>& clients, const std::vector<std::string>& msg, const Client& client, std::vector<Chanel>& chanels, const std::string& cmd);
static bool check_modo(Chanel *chanel, const Client& client);
static void mode_cmd(const std::vector<std::string>& cdm, Chanel *chanel);

// ==== Parser de commande ====
enum Command {
    CMD_EXIT,
    CMD_PING,
    CMD_PASS,
    CMD_NOPASS,
    CMD_NICK,
    CMD_NONICK,
    CMD_NOUSER,
    CMD_USER,
    CMD_JOIN,
    CMD_PART,
    CMD_PRIVMSG,
    CMD_KICK,
    CMD_INVITE,
    CMD_TOPIC,
    CMD_MODE,
    CMD_UNKNOWN
};

static Command parse_command(const char *buffer, const Client& client)
{
    if (!std::strcmp(buffer, "EXIT"))   return CMD_EXIT;
    if (!std::strcmp(buffer, "PING"))   return CMD_PING;
    if (!std::strcmp(buffer, "PASS"))   return CMD_PASS;
    if (!client.is_authenticated())     return CMD_NOPASS;
    if (!std::strcmp(buffer, "NICK"))   return CMD_NICK;
    if (client.get_nickname().empty())  return CMD_NONICK;
    if (!std::strcmp(buffer, "USER"))   return CMD_USER;
    if (client.get_username().empty())  return CMD_NOUSER;
    if (!std::strcmp(buffer, "JOIN"))   return CMD_JOIN;
    if (!std::strcmp(buffer, "PART"))   return CMD_PART;
    if (!std::strcmp(buffer, "PRIVMSG"))return CMD_PRIVMSG;
    if (!std::strcmp(buffer, "KICK"))   return CMD_KICK;
    if (!std::strcmp(buffer, "INVITE")) return CMD_INVITE;
    if (!std::strcmp(buffer, "TOPIC"))  return CMD_TOPIC;
    if (!std::strcmp(buffer, "MODE"))   return CMD_MODE;
    return CMD_UNKNOWN;
}

IRC_Serveur::IRC_Serveur(int port, const std::string &pass) : fd_server(-1), passwold(pass), running(false)
{
    // Socket creation
    fd_server = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_server < 0) {
        std::cerr << "Error socket creation" << std::endl;
        std::exit(1);
    }

    int opt = 1;
    setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port        = htons(port);

    if (bind(fd_server, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error binding association failed" << std::endl;
        close(fd_server);
        std::exit(1);
    }
    if (listen(fd_server, SOMAXCONN) < 0) {
        std::cerr << "Error listen" << std::endl;
        close(fd_server);
        std::exit(1);
    }
    std::cout << "Serveur IRC en ecoute sur le port " << port << "..." << std::endl;
}

std::string IRC_Serveur::get_password() const { return passwold; }

void IRC_Serveur::stop() {
    running = false;
    if (fd_server > 0) {
        shutdown(fd_server, SHUT_RDWR); // pour sortir de select/accept
    }
}

void IRC_Serveur::run()
{
    g_server_instance = this;
    signal(SIGINT, leave_irc);

    fd_set master_set, read_set;
    sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    FD_ZERO(&master_set);
    FD_SET(fd_server, &master_set);
    int max_fd = fd_server;

    running = true;

    while (running)
    {
        read_set = master_set;
        int sel = select(max_fd + 1, &read_set, NULL, NULL, NULL);
        if (sel < 0) {
            if (!running) break; // stop() a du casser select
            std::cerr << "select error" << std::endl;
            break;
        }

        // Arrivées
        if (FD_ISSET(fd_server, &read_set))
        {
            int client_fd = accept(fd_server, (struct sockaddr*)&address, &addrlen);
            if (client_fd >= 0) {
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(address.sin_addr), ip, INET_ADDRSTRLEN);
                clients.push_back(Client(client_fd, ip));
                FD_SET(client_fd, &master_set);
                if (client_fd > max_fd) max_fd = client_fd;
                std::cout << "Nouveau client connecte : " << client_fd << std::endl;
                send(client_fd, "Bienvenue sur le chat IRC !\n", 28, 0);
            } else {
                std::cout << "Error accept" << std::endl;
            }
        }

        // Clients existants
        for (size_t i = 0; i < clients.size(); ++i)
        {
            int fd = clients[i].get_fd_client();
            if (fd <= 0 || !FD_ISSET(fd, &read_set)) continue;

            char buffer[512];
            int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
            if (bytes <= 0)
            {
                std::cout << "Client deconnecte brutalement : " << clients[i].get_nickname() << std::endl;
                close(fd);
                FD_CLR(fd, &master_set);
                clients.erase(clients.begin() + i);
                --i;
                continue;
            }
            buffer[bytes] = '\0';

            std::string ircmsg = buffer;
            IRCMessage IRC = parseIRCMessage(ircmsg);
            std::cout << "commande du client " << clients[i].get_nickname() << " : " << buffer;

            Command cmd = parse_command(IRC.command.c_str(), clients[i]);

            switch (cmd)
            {
                case CMD_EXIT:
                    send(fd, "vous vous etes deconecter !\n", 28, 0);
                    std::cout << "Client deconnecte : " << fd << std::endl;
                    close(fd);
                    FD_CLR(fd, &master_set);
                    clients.erase(clients.begin() + i);
                    --i;
                    break;

                case CMD_PING:
                    send(fd, "PONG\n", 5, 0);
                    break;

                case CMD_PASS:
                    if (!IRC.params.empty() && IRC.params[0] == this->get_password()) {
                        clients[i].set_authenticated(true);
                        send(fd, "Mot de passe correct, vous pouvez continuer.\n", 45, 0);
                    } else {
                        send(fd, "Mot de passe incorrect, veillez reessayer.\n", 43, 0);
                    }
                    break;

                case CMD_NOPASS:
                    send(fd, "Vous devez d'abord entrer le mot de passe avec PASS.\n", 53, 0);
                    break;

                case CMD_NICK:
                    if (!IRC.params.empty())
                        change_nickname(clients, IRC.params[0], i);
                    else
                        send(fd, "Erreur: pseudo invalide (vide)\n", 31, 0);
                    break;

                case CMD_NONICK:
                    send(fd, "vous devez d'abord changer de nickname !\n", 41, 0);
                    break;

                case CMD_USER:
                    if (!IRC.params.empty())
                        change_username(clients[i], IRC.params[0], clients[i]);
                    else
                        send(fd, "Erreur: username invalide\n", 26, 0);
                    break;

                case CMD_NOUSER:
                    send(fd, "vous devez d'abord changer de username !\n", 41, 0);
                    break;

                case CMD_JOIN:
                {
                    for (size_t p = 0; p < IRC.params.size(); ++p)
                    {
                        std::string chanName = IRC.params[p];
                        if (chanName.empty()) continue;

                        Chanel *ch = set_chanel(chanels, chanName, true, clients[i]);
                        if (!ch) continue;

                        std::string mdp = (IRC.params.size() >= 2 ? IRC.params[1] : "");
                        join_chanel(clients[i], ch, mdp);
                    }
                    break;
                }

                case CMD_PART:
                {
                    for (size_t p = 0; p < IRC.params.size(); ++p)
                    {
                        Chanel *ch = set_chanel(chanels, IRC.params[p], false, clients[i]);
                        if (!ch) {
                            std::string reply = ":server 403 " + clients[i].get_nickname() + " "
                                              + IRC.params[p] + " :No such channel\r\n";
                            send(fd, reply.c_str(), reply.size(), 0);
                            break;
                        }
                        part_chanel(clients[i], ch, IRC.params[p]);
                    }
                    break;
                }

                case CMD_PRIVMSG:
                    if (!IRC.params.empty())
                        privmsg(clients, IRC.params, clients[i], chanels, IRC.command);
                    break;

                case CMD_KICK:
                {
                    if (IRC.params.empty()) break;
                    Chanel *ch = set_chanel(chanels, IRC.params[0], false, clients[i]);
                    if (!ch) {
                        std::string reply = ":server 403 " + clients[i].get_nickname() + " "
                                          + IRC.params[0] + " :No such channel\r\n";
                        send(fd, reply.c_str(), reply.size(), 0);
                        break;
                    }
                    if (check_modo(ch, clients[i])) {
                        for (size_t p = 1; p < IRC.params.size(); ++p)
                            ch->del_user(IRC.params[p]);
                        std::cout << "Client a retire quelqu'un" << std::endl;
                    }
                    break;
                }

                case CMD_INVITE:
                {
                    if (IRC.params.empty()) break;
                    Chanel *ch = set_chanel(chanels, IRC.params[0], false, clients[i]);
                    if (!ch) {
                        std::string reply = ":server 403 " + clients[i].get_nickname() + " "
                                          + IRC.params[0] + " :No such channel\r\n";
                        send(fd, reply.c_str(), reply.size(), 0);
                        break;
                    }
                    if (check_modo(ch, clients[i])) {
                        for (size_t ci = 0; ci < clients.size(); ++ci) {
                            for (size_t p = 1; p < IRC.params.size(); ++p)
                                if (clients[ci].get_username() == IRC.params[p])
                                    ch->add_user(clients[ci]);
                        }
                        std::cout << "Client a invite quelqu'un" << std::endl;
                    }
                    break;
                }

                case CMD_TOPIC:
                {
                    if (IRC.params.size() < 2) break;
                    Chanel *ch = set_chanel(chanels, IRC.params[0], false, clients[i]);
                    if (!ch) {
                        std::string reply = ":server 403 " + clients[i].get_nickname() + " "
                                          + IRC.params[0] + " :No such channel\r\n";
                        send(fd, reply.c_str(), reply.size(), 0);
                        break;
                    }
                    const std::vector<Client*>& users = ch->get_user();
                    if (check_modo(ch, clients[i])) {
                        std::string txt = ":" + clients[i].get_nickname() + " TOPIC #" + ch->get_name() + " : " + IRC.params[1] + "\n";
                        for (size_t u = 0; u < users.size(); ++u)
                            if (users[u]) send(users[u]->get_fd_client(), txt.c_str(), txt.size(), 0);
                        std::cout << "Client a change le theme." << std::endl;
                    }
                    break;
                }

                case CMD_MODE:
                {
                    if (IRC.params.empty()) break;
                    Chanel *ch = set_chanel(chanels, IRC.params[0], false, clients[i]);
                    if (!ch) {
                        std::string reply = ":server 403 " + clients[i].get_nickname() + " "
                                          + IRC.params[0] + " :No such channel\r\n";
                        send(fd, reply.c_str(), reply.size(), 0);
                        break;
                    }
                    if (check_modo(ch, clients[i])) {
                        mode_cmd(IRC.params, ch);
                        std::cout << "Client a ouvert les parametres" << std::endl;
                    }
                    break;
                }

                case CMD_UNKNOWN:
                    std::cout << "Unknown commande" << std::endl;
                    break;
            }
        }
    }

    // sortie de run(): le destructeur fera le nettoyage
}

IRC_Serveur::~IRC_Serveur()
{
    std::cout << "Shutting down IRC server..." << std::endl;

    running = false;

    // Fermer tous les clients
    for (size_t i = 0; i < clients.size(); ++i) {
        int cfd = clients[i].get_fd_client();
        if (cfd > 0) {
            shutdown(cfd, SHUT_RDWR);
            close(cfd);
            clients[i].mark_fd_closed();
        }
    }
    clients.clear();

    // Fermer le socket serveur
    if (fd_server > 0) {
        shutdown(fd_server, SHUT_RDWR);
        close(fd_server);
        fd_server = -1;
    }

    chanels.clear();

    std::cout << "IRC server stopped and cleaned up." << std::endl;
}

// ======================= Implémentations helpers =======================

static std::string send_prefix(const Client& client)
{
    return ":" + client.get_nickname() + "!" + client.get_username() + "@" + client.get_ip() + " ";
}

static void send_msg(const Client& client, const std::vector<std::string>& msg, int fd, const std::string& cmd)
{
    if (msg.empty()) return;
    std::string txt = send_prefix(client) + cmd + " " + msg[0] + " : ";
    send(fd, txt.c_str(), txt.size(), 0);

    for (size_t i = 1; i < msg.size(); ++i) {
        send(fd, msg[i].c_str(), msg[i].size(), 0);
        send(fd, " ", 1, 0);
    }
    send(fd, "\r\n", 2, 0);
}

static void change_nickname(std::vector<Client>& clients, const std::string& nick, size_t i)
{
    if (nick.empty()) {
        send(clients[i].get_fd_client(), "Erreur: pseudo invalide (vide)\n", 31, 0);
        return;
    }

    if (!clients[i].get_nickname().empty()) {
        send(clients[i].get_fd_client(), "vous ne pouvez pas changer de nickname !\n", 41, 0);
        return;
    }

    for (size_t j = 0; j < clients.size(); ++j) {
        if (j != i && clients[j].get_nickname() == nick) {
            std::string txt = ":server 433 * " + nick + " :Nickname is already in use\n";
            send(clients[i].get_fd_client(), txt.c_str(), txt.size(), 0);
            return;
        }
    }

    clients[i].set_nickname(nick);
    send(clients[i].get_fd_client(), "vous avez change de pseudo !\n", 30, 0);
    std::cout << "Client " << clients[i].get_fd_client()
              << " a change son pseudo en: " << nick << std::endl;
}

static void change_username(Client &target, const std::string &user, const Client &self)
{
    std::string trimmed = user;
    size_t end = trimmed.find_last_not_of(" \r\n");
    if (end != std::string::npos) trimmed.erase(end + 1);

    if (trimmed.empty()) {
        send(target.get_fd_client(), "Erreur: pseudo invalide\n", 25, 0);
        return;
    }

    target.set_username(trimmed);
    std::string txt = ":server 001 " + trimmed + " :Welcome to the IRC server " + send_prefix(self) + "\n";
    send(target.get_fd_client(), txt.c_str(), txt.size(), 0);
    std::cout << "Client " << target.get_fd_client() << " a change son username en: " << trimmed << std::endl;
}

static void join_chanel(Client &client, Chanel *chanel, const std::string &mdp)
{
    const std::vector<Client*>& users = chanel->get_user();

    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i] && users[i]->get_nickname() == client.get_nickname()) {
            std::string txt = "Vous etes deja dans ce channel !\n";
            send(client.get_fd_client(), txt.c_str(), txt.size(), 0);
            return;
        }
    }
    if (chanel->get_invite() == true)
        send(client.get_fd_client(), "chanel sur invitation uniqument !\n", 34, 0);
    else if (chanel->get_size() != static_cast<size_t>(-1) && users.size() >= chanel->get_size())
        send(client.get_fd_client(), "le chanel est plein !\n", 22, 0);
    else if (!chanel->get_mdp().empty() && chanel->get_mdp() != mdp)
        send(client.get_fd_client(), "le mot de pass est incorect !\n", 30, 0);
    else {
        std::string txt = ":" + client.get_nickname() + " JOIN :#" + chanel->get_name() + "\n";
        for (size_t i = 0; i < users.size(); ++i)
            if (users[i]) send(users[i]->get_fd_client(), txt.c_str(), txt.size(), 0);
        chanel->add_user(client);
        send(client.get_fd_client(), "Vous avez rejoint le channel !\n", 32, 0);
    }
}

static void part_chanel(Client &client, Chanel *chanel, const std::string &name)
{
    if (!chanel) {
        send(client.get_fd_client(), "Channel introuvable !\n", 23, 0);
        std::cout << "Channel introuvable : " << name << std::endl;
        return;
    }

    const std::vector<Client*>& users = chanel->get_user();
    bool inside = false;
    for (size_t i = 0; i < users.size(); ++i)
        if (users[i] && users[i]->get_nickname() == client.get_nickname())
            inside = true;

    if (!inside) {
        send(client.get_fd_client(), "Vous n'etes pas dans ce channel !\n", 35, 0);
        std::cout << "Client " << client.get_nickname() << " a essaye de quitter " << name << " sans y etre.\n";
        return;
    }

    chanel->del_user(client.get_nickname());
    std::string txt = ":" + client.get_nickname() + " PART #" + chanel->get_name() + "\n";
    for (size_t i = 0; i < users.size(); ++i)
        if (users[i]) send(users[i]->get_fd_client(), txt.c_str(), txt.size(), 0);
    send(client.get_fd_client(), "Vous avez quitte le channel.\n", 31, 0);
    std::cout << "Client " << client.get_nickname() << " a quitte le channel " << name << std::endl;
}

static void send_in_chanel(const std::vector<std::string>& msg, const Client& client, Chanel& chanel)
{
    const std::vector<Client*>& users = chanel.get_user();
    bool inside = false;
    for (size_t i = 0; i < users.size(); ++i)
        if (users[i] && users[i]->get_nickname() == client.get_nickname())
            inside = true;

    if (!inside) {
        send(client.get_fd_client(), "Vous n'etes pas dans ce channel !\n", 35, 0);
        return;
    }
    for (size_t i = 0; i < users.size(); ++i)
        if (users[i]) send_msg(client, msg, users[i]->get_fd_client(), "PRIVMSG");
}

static void privmsg(const std::vector<Client>& clients, const std::vector<std::string>& msg, const Client& client, std::vector<Chanel>& chanels, const std::string& cmd)
{
    if (msg.empty()) return;

    if (!msg[0].empty() && msg[0][0] == '#') {
        std::string name = msg[0].substr(1);
        for (size_t i = 0; i < chanels.size(); ++i) {
            if (chanels[i].get_name() == name) {
                send_in_chanel(msg, client, chanels[i]);
                return;
            }
        }
    }

    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].get_nickname() == msg[0]) {
            send_msg(client, msg, clients[i].get_fd_client(), cmd);
            return;
        }
    }
    std::string txt = ":server 401 " + client.get_nickname() + " " + msg[0] + " :No such nick/channel\n";
    send(client.get_fd_client(), txt.c_str(), txt.size(), 0);
}

static bool check_modo(Chanel *chanel, const Client& client)
{
    const std::vector<std::string> &md = chanel->get_modo();
    for (size_t j = 0; j < md.size(); ++j)
        if (client.get_nickname() == md[j]) return true;

    std::string reply = ":server 482 " + client.get_nickname() + " #" + chanel->get_name() + " :You're not channel operator\r\n";
    send(client.get_fd_client(), reply.c_str(), reply.size(), 0);
    return false;
}

static void mode_cmd(const std::vector<std::string>& cdm, Chanel *chanel)
{
    if (cdm.size() < 2) return;

    if (cdm[1].size() == 2 && cdm[1][1] == 'i') {
        if (cdm[1][0] == '+') chanel->chanel_only_invite(true);
        if (cdm[1][0] == '-') chanel->chanel_only_invite(false);
    }
    if (cdm[1].size() == 2 && cdm[1][1] == 't') {
        if (cdm[1][0] == '+') chanel->set_topic(true);
        if (cdm[1][0] == '-') chanel->set_topic(false);
    }
    if (cdm[1].size() == 2 && cdm[1][1] == 'k') {
        if (cdm[1][0] == '+') { if (cdm.size() >= 3) chanel->set_mdp(cdm[2]); }
        if (cdm[1][0] == '-') chanel->set_mdp("");
    }
    if (cdm[1].size() == 2 && cdm[1][1] == 'o') {
        if (cdm[1][0] == '+') { if (cdm.size() >= 3) chanel->add_modo(cdm[2]); }
        if (cdm[1][0] == '-') { if (cdm.size() >= 3) chanel->del_modo(cdm[2]); }
    }
    if (cdm[1].size() == 2 && cdm[1][1] == 'l') {
        if (cdm[1][0] == '+') { if (cdm.size() >= 3) chanel->set_size(cdm[2]); }
        if (cdm[1][0] == '-') chanel->set_size("-1");
    }
}
