#pragma once

#include "Server.hpp"
#include "Commands.hpp"

std::string ERR_NONICKNAMEGIVEN(std::string error);
std::string ERR_ERRONEUSNICKNAME(std::string name);
std::string ERR_NICKNAMEINUSE(std::string name);
std::string ERR_UNKNOWNCOMMAND(User *user, std::string command);

std::string ERR_NONICKNAMEGIVEN(std::string name) {
	return "431 <" + name + "> :No nickname given";
}

std::string ERR_ERRONEUSNICKNAME(std::string name) {
	return "432 <" + name + "> :Erroneus nickname";
}

std::string ERR_NICKNAMEINUSE(std::string name) {
	return "433 <" + name + "> :Nickname is already in use";
}

std::string ERR_UNKNOWNCOMMAND(User *user, std::string command) {
	return "421 <" + user->get_username() + "> <" + command + " > :Unknown command";
	// Check si c'est le username ou autre chose a afficher car <client> normalement
}
