#pragma once
#include "Server.hpp"
#include "Commands.hpp"

std::string ERR_NONICKNAMEGIVEN( std::string error );
std::string ERR_ERRONEUSNICKNAME (std::string name);
std::string ERR_NICKNAMEINUSE ( std::string name );


std::string ERR_NONICKNAMEGIVEN( std::string name ){
	return "431 <" + name + "> :No nickname given";
}

std::string ERR_ERRONEUSNICKNAME ( std::string name ) {
	return "432 <" + name + "> :Erroneus nickname";
}
std::string ERR_NICKNAMEINUSE ( std::string name ) {
	return "433 <" + name + "> :Nickname is already in use";
}
