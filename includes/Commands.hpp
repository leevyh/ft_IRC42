#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <time.h>
#include "IRC.hpp"

class Server;
class User;
class Channel;
class Commands {
	public:
		Commands(void);
		~Commands(void);
		typedef void (Commands::*cmdFPtr)(Server &server, User &user, std::vector<std::string> &arguments);
		void getcommand(Server &server, User &user, std::vector<std::string> &arguments);
/* Connexion Registration */
		void capls(Server &server, User &user, std::vector<std::string> &arg);
		void pass(Server &server, User &user, std::vector<std::string> &arg);
		void nick(Server &server, User &user, std::vector<std::string> &arg);
		void user(Server &server, User &user, std::vector<std::string> &arg);
		void quit(Server &server, User &user, std::vector<std::string> &arg);
		void whois(Server &server, User &user, std::vector<std::string> &arg);
/* Channel operations */
		void join(Server &server, User &user, std::vector<std::string> &arg);
		void invite(Server &server, User &user, std::vector<std::string> &arg);
		void kick(Server &server, User &user, std::vector<std::string> &arg);
		void part(Server &server, User &user, std::vector<std::string> &arg);
		void topic(Server &server, User &user, std::vector<std::string> &arg);
		void mode(Server &server, User &user, std::vector<std::string> &arg);
		void who(Server &server, User &user, std::vector<std::string> &arg);

		void privmsg(Server &server, User &user, std::vector<std::string> &arg);
		void ping(Server &server, User &user, std::vector<std::string> &arg);
		void pong(Server &server, User &user, std::vector<std::string> &arg);
	private:
		std::map<std::string, cmdFPtr> cmdMap;
};

int		check_channelName(Server &server, User &user, std::vector<std::string> &channel);
std::vector<std::string> split(const std::string& str);
void	create_NewChannel(Server &server, User &user, std::string const &channel_name);
void	edit_Topic(Server &server, User &user, std::vector<std::string> &arg, Channel &chan);
std::string remove_OneChar(char c, std::vector<std::string> &arg, int i);
short is_Authorize(User &user, Channel &channel, std::string key);
std::string print_Names(std::string nickname, Channel &channel);
