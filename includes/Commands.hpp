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
/* Connection Registration */
		void capls(Server &server, User &user, std::vector<std::string> &arg);
		void pass(Server &server, User &user, std::vector<std::string> &arg);
		void nick(Server &server, User &user, std::vector<std::string> &arg);
		void user(Server &server, User &user, std::vector<std::string> &arg);
		void quit(Server &server, User &user, std::vector<std::string> &arg);
/* Channel operations */
		void join(Server &server, User &user, std::vector<std::string> &arg);
		void kick();
		void invite(Server &server, User &user, std::vector<std::string> &arg);
		void topic(Server &server, User &user, std::vector<std::string> &arg);
		void mode(Server &server, User &user, std::vector<std::string> &arg);
		void set_mode(Server &server, User &user, Channel &chan, std::vector<std::string> &arg);
		void unset_mode(Server &server, User &user, Channel &chan, std::vector<std::string> &arg);

		void part();
		void privmsg(Server &server, User &user, std::vector<std::string> &arg);
		void ping(Server &server, User &user, std::vector<std::string> &arg);
		void pong(Server &server, User &user, std::vector<std::string> &arg);
	private:
		std::map<std::string, cmdFPtr> cmdMap;
};

int		check_channelName(Server &server, User &user, std::vector<std::string> &channel);
std::vector<std::string> split(const std::string& str);
void	create_NewChannel(Server &server, User &user, std::string const &channel_name);
void	add_UserInChannel(Server &server, User &user, Channel &channel);
void	edit_Topic(Server &server, User &user, std::vector<std::string> &arg, Channel &chan);
