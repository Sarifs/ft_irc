#ifndef PARSING_HPP
#define PARSING_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

struct  IRCMessage{
    std::string     prefix;
    std::string     command;
    std::vector<std::string>    params;
};


IRCMessage  parseIRCMessage(const std::string& rawInput);
void    handleIRCMessage(const IRCMessage& msg);


#endif