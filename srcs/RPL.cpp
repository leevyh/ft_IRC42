#include "IRC.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "Commands.hpp"

void displayWelcome( Server& server, User& user ) {
	std::cout << "display welcome \nuser fd:" << user.get_fd() << std::endl;
	std::cout << "String :" << RPL_WELCOME(server, user).c_str() << std::endl;
	std::cout << "Size:" << RPL_WELCOME(server, user).size() << std::endl;

	if (send(user.get_fd(), RPL_WELCOME(server, user).c_str(), RPL_WELCOME(server, user).size(), 0) == -1)
    	// send(user.getfd(), RPL_YOURHOST(server, client).c_str(), RPL_YOURHOST(server, client).length(), 0) == -1 ||
        // send(user.getfd(), RPL_CREATED(server, client).c_str(), RPL_CREATED(server, client).length(), 0) == -1 ||
    	// send(user.getfd(), RPL_MYINFO(server, client).c_str(), RPL_MYINFO(server, client).length(), 0) == -1)
		std::perror("send:");
}

// 001 Neoblack :Welcome to the DALnet IRC Network Neoblack!~amugnier@62.210.33.34
std::string RPL_WELCOME( Server& server, User& user ) {
	return ("001 " + user.get_nickname() + " :Welcome to the "
	+ server.get_networkname() + " Network, " + user.get_nickname()
	+ "!~" + user.get_username() + "@" + user.get_ip() + "\r\n");
}

std::string ERR_NONICKNAMEGIVEN( std::string name ) {
	return ("431 " + name + " :No nickname given");
}

std::string ERR_ERRONEUSNICKNAME( std::string name ) {
	return ("432 " + name + " :Erroneus nickname");
}

std::string ERR_NICKNAMEINUSE( std::string name ) {
	return ("433 " + name + " :Nickname is already in use");
}

std::string ERR_UNKNOWNCOMMAND( User& user, std::string command ) {
	return ("421 " + user.get_username() + " " + command + " :Unknown command");
}

std::string ERR_NEEDMOREPARAMS( User& user, std::string command ) {
	return ("461 " + user.get_username() + " " + command + " :Not enough parameters");
}

std::string ERR_ALREADYREGISTRED( User& user ) {
	return ("462 " + user.get_username() + " :You may not reregister");
}
