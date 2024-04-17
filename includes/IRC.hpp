#pragma once

#include <iostream>
#include <string>
#include <Server.hpp>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <vector>
#include <poll.h>
#include <sys/types.h>
#include "Server.hpp"
#include "User.hpp"
#include "Commands.hpp"
#include "Channel.hpp"

extern bool signal_value;
void check_args(int argc, char **argv);
void signal_send(int signum);

// IRC PROTOCOL
void displayWelcome(Server &server, User &user);
std::string RPL_WELCOME(Server &server, User &user);                   // 001
std::string RPL_YOURHOST(Server &server, User &user);                  // 002
std::string RPL_CREATED(User &user);                                   // 003
std::string RPL_MYINFO(Server &server, User &user);                    // 004

std::string RPL_AWAY(User &user, std::string recipient, std::string message); // 301

std::string ERR_NOSUCHNICK(User &user, std::string nickname);          // 401
std::string ERR_NOSUCHSERVER(User &user, std::string server_name);     // 402
std::string ERR_NOSUCHCHANNEL(User &user, std::string channel);        // 403
std::string ERR_CANNOTSENDTOCHAN(User &user, std::string channel);     // 404
std::string ERR_NOORIGIN(User &user);                                  // 409
std::string ERR_NORECIPIENT(User &user, std::string command);          // 411
std::string ERR_NOTEXTTOSEND(User &user);                              // 412
std::string ERR_NONICKNAMEGIVEN(std::string error);                    // 431
std::string ERR_ERRONEUSNICKNAME(std::string name);                    // 432
std::string ERR_NICKNAMEINUSE(std::string name);                       // 433
std::string ERR_UNKNOWNCOMMAND(User &user, std::string command);       // 421
std::string ERR_NOTONCHANNEL(User &user, Channel &chan);               // 442
std::string ERR_USERONCHANNEL(User &user, std::string to_join, Channel &chan); // 443
std::string ERR_NEEDMOREPARAMS(User &user, std::string command);       // 461
std::string ERR_ALREADYREGISTRED(User &user);                          // 462
std::string ERR_BADCHANMASK(std::string channel_mask);                 // 476


struct IsClientFDPredicate {
	int clientFD;

	IsClientFDPredicate(int fd) : clientFD(fd) {}

	bool operator()(const pollfd& pfd) const {
		return pfd.fd == clientFD;
	}
};
