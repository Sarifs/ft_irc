#include "Parsing.hpp"

extern  IRCMessage  parseIRCMessage(const std::string& raw);

// int main(){
//     std::string input = ":Jean!~jean@localhost PRIVMSG #general : Salut tout le monde !\r\n";

//     IRCMessage msg = parseIRCMessage(input);

//     std::cout << "Prefix : " << msg.prefix << "\n";
//     std::cout << "Command : " << msg.command << "\n";
//     std::cout << "Params :\n";
//     for (size_t i = 0; i < msg.params.size(); i++){
//         std::cout << " [" << i << "] " << msg.params[i] << "\n";
//     }

//     return 0;
// }

int main() {
    // Simule une réponse PING
    handleIRCMessage({"", "PING", {"123456"}});

    // Simule un message PONG
    handleIRCMessage({"", "PONG", {}});

    // Simule un message PRIVMSG
    handleIRCMessage({"nick!user@host", "PRIVMSG", {"#channel", "Hello everyone!"}});

    // Simule un message NOTICE
    handleIRCMessage({"server", "NOTICE", {"#channel", "This is a notice."}});

    // Simule un JOIN
    handleIRCMessage({"nick!user@host", "JOIN", {"#channel"}});

    // Simule un PART
    handleIRCMessage({"nick!user@host", "PART", {"#channel"}});

    // Simule un QUIT
    handleIRCMessage({"nick!user@host", "QUIT", {"Goodbye!"}});

    // Simule un changement de nick
    handleIRCMessage({"oldnick!user@host", "NICK", {"newnick"}});

    // Simule un changement de topic
    handleIRCMessage({"nick!user@host", "TOPIC", {"#channel", "New topic of the channel"}});

    // Simule un changement de mode
    handleIRCMessage({"nick!user@host", "MODE", {"#channel", "+o", "nick"}});

    // Simule un message 001 (welcome)
    handleIRCMessage({"", "001", {"nick", "Welcome to IRC!"}});

    // Simule une erreur de nick déjà pris
    handleIRCMessage({"", "433", {"*", "nick", "Nickname is already in use."}});

    // Simule une commande inconnue
    handleIRCMessage({"", "XYZ", {"some", "random", "params"}});

    return 0;
}