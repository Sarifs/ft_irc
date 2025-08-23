#include "Client.hpp"

void sendThread(int sock, ClientState& state) {
    while (state.running){
        std::string msg;
        {
            std::lock_guard<std::mutex> lock(state.sendMutex);
            if (!state.sendQueue.empty()){
                msg = state.sendQueue.front();
                state.sendQueue.pop();
            }
        }

        if (!msg.empty()){
            ssize_t sent = send(sock, msg.c_str(), msg.size(), 0);
            if (sent <= 0){
                std::cerr << "Erreur d'envoi ou déconnexion serveur.\n";
                state.running = false;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int     connectToServer(const std::string& host, int port){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        perror("socket");
        return -1;
    }
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0){
        perror("inet_pton");
        close(sock);
        return -1;
    }
    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        perror("connect");
        close(sock);
        return -1;
    }
    return sock;
}

void    receiveLoop(int sock, ClientState& state){
    char buffer[512];
    std::string leftover;

    while (state.running){
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0){
            std::cout << "❌ Déconnecté du serveur.\n";
            state.running = false;
            break;
        }
        leftover += std::string(buffer, bytes);

        size_t pos;
        while ((pos = leftover.find("\r\n")) != std::string::npos){
            std::string line = leftover.substr(0, pos + 2);
            leftover.erase(0, pos + 2);
            try{
                IRCMessage msg = parseIRCMessage(line);
                handleIRCMessage(msg);
                if (msg.command == "PING" && !msg.params.empty()) {
                    std::string pong = "PONG :" + msg.params[0] + "\r\n";
                    std::lock_guard<std::mutex> lock(state.sendMutex);
                    state.sendQueue.push(pong);
                }
            } catch (const std::exception& e){
                std::cerr << "Erreur parsing message IRC: " << e.what() << "\n";
            }
        }
    }
}

void    userInputLoop(ClientState& state){
    while (state.running){
        if (std::cin.peek() != EOF){
            std::string input;
            std::getline(std::cin, input);
            if (!input.empty()){
                if (input == "/quit"){
                    std::lock_guard<std::mutex> lock(state.sendMutex);
                    state.sendQueue.push("QUIT: Leaving\r\n");
                    state.running = false;
                    break;
                } else {
                    input += "\r\n";
                    std::lock_guard<std::mutex> lock(state.sendMutex);
                    state.sendQueue.push(input);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main() {

    ClientState state;
    int sock = connectToServer("127.0.0.1", 6667);
    if (sock < 0) return 1;

    std::cout << "✅ Connecté au serveur IRC.\n";

    {
        std::lock_guard<std::mutex> lock(state.sendMutex);
        state.sendQueue.push("NICK testnick\r\n");
        state.sendQueue.push("USER testuser 0 * :Real Name\r\n");
    }

    std::thread tRecv(receiveLoop, sock, std::ref(state));
    std::thread tSend(sendThread, sock, std::ref(state));

    userInputLoop(state);

    state.running = false;
    tRecv.join();
    tSend.join();

    close(sock);
    std::cout << "Client fermé proprement.\n";
    return 0;
}


