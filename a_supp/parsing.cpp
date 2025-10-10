#include "Parsing.hpp"

IRCMessage  parseIRCMessage(const std::string& rawInput){
    IRCMessage  msg;

    std::string line = rawInput;
    if (line.size() >= 2 && line.substr(line.size() - 2) == "\r\n")
        line = line.substr(0, line.size() - 2);
    else if (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
        line.pop_back();
    
        std::istringstream iss(line);
        std::string token;

    if (!line.empty() && line[0] == ':'){
            iss >> token;
            msg.prefix = token.substr(1);
    }

    if (!(iss >> msg.command)){
            msg.command = "";
            return msg;
    }

    while (iss >> token){
        if (token[0] == ':'){
            std::string param = token.substr(1);
            std::string rest;
            while (iss >> rest)
                param += " " + rest;
            msg.params.push_back(param);
            break;
        } else{
            msg.params.push_back(token);
        }
    }
    return msg;
}


void    handleIRCMessage(const IRCMessage& msg){
    const std::string&  cmd = msg.command;

    if (cmd == "PING"){
        if (!msg.params.empty()){
            std::cout << "PING :" << msg.params[0] << "\r\n";
        }
    } else if (cmd == "PONG"){
        std::cout << "Received PONG from server\n";
    } else if (cmd == "PRIVMSG"){
        if (msg.params.size() >= 2){
            std::string from = msg.prefix;
            std::string target = msg.params[0];
            std::string message = msg.params[1];

            std::cout << "Message from "<< from << " to " << target << ": " << message << "\n";
        }
    } else if (cmd == "NOTICE"){
        if (msg.params.size() >= 2){
            std::string from = msg.prefix;
            std::string target = msg.params[0];
            std::string message = msg.params[1];

            std::cout << "Notice from "<< from << " to " << target << ": " << message << "\n";
        }
    } else if (cmd == "JOIN"){
        if (!msg.params.empty()){
            std::cout << msg.prefix << " joined " << msg.params[0] << "\n";
        }
    } else if (cmd == "PART"){
        if (!msg.params.empty()){
            std::cout << msg.prefix << " left " << msg.params[0] << "\n";
        }
    } else if (cmd == "QUIT"){
        std::string reason = (msg.params.empty() ? "" : msg.params[0]);
        std::cout << msg.prefix << " quit IRC (" << reason << ")\n";
    } else if (cmd =="NICK"){
        if (!msg.params.empty()){
            std::cout << msg.prefix << " is now known as " << msg.params[0] << "\n";
        }
    } else if (cmd == "TOPIC"){
        if (msg.params.size() >= 2){
            std::cout << msg.prefix << " changed topic on " << msg.params[0] << " to: " << msg.params[1] << "\n";
        }
    } else if (cmd == "MODE"){
        std::cout << msg.prefix << " set mode ";
        for (const auto& param : msg.params) std::cout << param << " ";
        std::cout << "\n";
    } else if (cmd == "001"){
        std::cout << "Connected: " << msg.params.back() << "\n";
    } else if (cmd == "433"){
        std::cout << "Error: Nickname is already in use.\n";
    } else {
        std::cout << "Unhandled command [" << cmd << "]\n";
        std::cout << "Params: ";
        for (const auto& param : msg.params){
            std::cout << "[" << param << "] ";
        }
        std::cout << "\n";
    }
}
