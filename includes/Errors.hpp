#pragma once
#include "IRC.hpp"

// std::string ERR_NONICKNAMEGIVEN( std::string error );					// 431
// std::string ERR_ERRONEUSNICKNAME( std::string name );					// 432
// std::string ERR_NICKNAMEINUSE( std::string name );						// 433
// std::string ERR_UNKNOWNCOMMAND( User& user, std::string command );		// 421
// std::string ERR_NEEDMOREPARAMS( User& user, std::string command );		// 461
// std::string ERR_ALREADYREGISTRED( User& user );							// 462

std::string ERR_NONICKNAMEGIVEN( std::string name ) {
	return ("431 " + name + " :No nickname given");
}

std::string ERR_ERRONEUSNICKNAME( std::string name ) {
	return ("432 " + name + " :Erroneus nickname");
}

std::string ERR_NICKNAMEINUSE( std::string name ) {
	return ("433 " + name + " :Nickname is already in use");
}

std::string ERR_UNKNOWNCOMMAND( User* user, std::string command ) {
	return ("421 " + user->get_username() + " " + command + " :Unknown command");
}

std::string ERR_NEEDMOREPARAMS( User* user, std::string command ) {
	return ("461 " + user->get_username() + " " + command + " :Not enough parameters");
}

std::string ERR_ALREADYREGISTRED( User* user ) {
	return ("462 " + user->get_username() + " :You may not reregister");
}
