#include "Client.hpp"

int main(int ac, char **av)
{
    if (ac != 2 || !atoi(av[1]))
        return 0;

    Client client[10];
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(6667);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    fd_set master_set, read_set;
    int max_fd = server_fd;
    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);

    int clients[FD_SETSIZE] = {0};

    std::cout << "Serveur en écoute sur le port 6667...\n";

    while (1)
    {
        read_set = master_set;
        if (select(max_fd + 1, &read_set, nullptr, nullptr, nullptr) < 0)
        {
            perror("select");
            break;
        }

        for (int fd = 0; fd <= max_fd; fd++)
        {
            if (FD_ISSET(fd, &read_set))
            {
                if (fd == server_fd)
                {
                    int client_fd = accept(server_fd, nullptr, nullptr);
                    client[fd].set_id(fd);
                    FD_SET(client_fd, &master_set);

                    if (client_fd > max_fd)
                        max_fd = client_fd;
                    std::cout << "Nouveau client connecté : " << client_fd << std::endl;
                    send(client_fd, "Bienvenue sur le chat IRC !\n", 28, 0);
                }
                else
                {
                    char buffer[512];
                    int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
                    if (bytes <= 0)
                    {
                        std::cout << "Client déconnecté : " << fd << std::endl;
                        close(fd);
                        FD_CLR(fd, &master_set);
                    }
                    else
                    {
                        buffer[bytes - 1] = '\0';
                        std::cout << "Message du client " << client[fd].get_nickname() << " : " << buffer << std::endl;

                        if (!strncmp(buffer, "EXIT", 4))
                        {
                            send(fd, "vous vous etes deconecter !", 27, 0);
                            std::cout << "Client déconnecté : " << fd << std::endl;
                            close(fd);
                            FD_CLR(fd, &master_set);
                        }

                        else if (!strncmp(buffer, "PING", 4))
                            send(fd, "PONG\n", 5, 0);

                        else if (!strncmp(buffer, "NICK ", 5))
                        {
                            send(fd, "vous avez changer de pseudo !\n", 30, 0);
                            std::cout << "Client a changer son pseudo." << std::endl;
                            client[fd].set_nickname(&buffer[5]);
                        }

                        else if (client[fd].get_nickname() == "nickname")
                            send(fd, "veuillier d'abord changer votre pseudo en utulisent la commande 'NICK' suivie de votre pseudo !\n", 96, 0);
                        
                        else if (!strncmp(buffer, "USER ", 5))
                        {
                            send(fd, "vous avez changer de nom d'utilisateur !\n", 41, 0);
                            std::cout << "Client a changer son nom d'utilisateur." << std::endl;
                            client[fd].set_username(&buffer[5]);
                        }
                        
                        else if (client[fd].get_username() == "username")
                            send(fd, "veuillier d'abord changer votre nom d'utilisateur en utulisent la commande 'USER' suivie de votre nom d'utilisateur !\n", 118, 0);
                        
                        else if (!strncmp(buffer, "JOIN ", 5))
                        {
                            if (client[fd].get_chanelname() != "void")
                                std::cout << "Client a quitter le chanel." << std::endl;
                            send(fd, "vous avez changer de canal !\n", 29, 0);
                            client[fd].set_chanelname(&buffer[5]);
                            std::cout << "Client a rejoins le chanel " << &buffer[5] << " ." << std::endl;
                        }

                        else if (client[fd].get_chanelname() == "void")
                            send(fd, "veuillier d'abord enter dans un salon en utulisent la commande 'JOIN' suivie de votre nom du salon !\n", 111, 0);
                        
                        else if (!strncmp(buffer, "PART ", 5))
                        {
                            if (client[fd].get_chanelname() == "void")
                            send(fd, "vous etes dans aucun chanel !\n", 30, 0);
                            else
                            {
                                client[fd].set_chanelname("void");
                                send(fd, "vous avez quitter le canal !\n", 29, 0);
                            }
                        }

                        else if (!strncmp(buffer, "KICK", 4))
                            std::cout << "Client a retirer quelqu'un" << std::endl;

                        else if (!strncmp(buffer, "INVITE", 4))
                            std::cout << "Client a invité quelqu'un" << std::endl;

                        else if (!strncmp(buffer, "TOPIC", 4))
                            std::cout << "Client a changer le theme." << std::endl;

                        else if (!strncmp(buffer, "MODE", 4))
                            std::cout << "Client a ouvert les parametre" << std::endl;

                        else
                        {
                            for (int other_fd = 0; other_fd <= max_fd; other_fd++)
                                if (FD_ISSET(other_fd, &master_set) && client[other_fd].get_chanelname() == client[fd].get_chanelname() && other_fd != server_fd && other_fd != fd)
                                    send(other_fd, buffer, bytes, 0);
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
}
