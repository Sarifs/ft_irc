

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <queue>
//#include <thread>
//#include <mutex>
//#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "Parsing.hpp"

/* struct ClientState{
    std::mutex      sendMutex;
    std::queue<std::string>  sendQueue;
    std::atomic<bool>   running{true};
}; */

// void    sendThread(int sock, ClientState& state);
// int     conenctToServer(const std::string& host, int port);
// void    receiveLoop(int sock, ClientState& state);
// void    userInputLoop(ClientState&  state);

#endif
