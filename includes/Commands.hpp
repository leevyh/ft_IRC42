#pragma once

#include <iostream>
#include <string>
#include <vector>
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
		void part();
		void topic();
		void names();
		void invite();
		void leave();
		void kick();
		void mode(Server &server, User &user, std::vector<std::string> &arg);
		void privmsg(Server &server, User &user, std::vector<std::string> &arg);

		void ping(Server &server, User &user, std::vector<std::string> &arg); // void pong() ??
	private:
		std::map<std::string, cmdFPtr> cmdMap;
};

int		check_channelName(Server &server, User &user, std::vector<std::string> &channel);
std::vector<std::string> split(const std::string& str);
void	create_NewChannel(Server &server, User &user, std::string const &channel_name);
void	add_UserInChannel(User &user, std::string const &channel_name, std::map<std::string, Channel>::iterator it);
