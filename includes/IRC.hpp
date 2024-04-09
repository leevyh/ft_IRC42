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

extern bool signal_value;
void check_args(int argc, char **argv);
void signal_send(int signum);
// IRC PROTOCOL
void displayWelcome(Server &server, User &user);
std::string RPL_WELCOME(Server &server, User &user);                    // 001
std::string ERR_NONICKNAMEGIVEN(std::string error);                    // 431
std::string ERR_ERRONEUSNICKNAME(std::string name);                    // 432
std::string ERR_NICKNAMEINUSE(std::string name);                        // 433
std::string ERR_UNKNOWNCOMMAND(User &user, std::string command);        // 421
std::string ERR_NEEDMOREPARAMS(User &user, std::string command);        // 461
std::string ERR_ALREADYREGISTRED(User &user);                            // 462