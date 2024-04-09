#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "IRC.hpp"

class Server;
class User;
class Commands {
	public:
		Commands(void);
		~Commands(void);
		typedef void (Commands::*cmdFPtr)(Server &server, User &user, std::vector<std::string> &arguments);
		void getcommand(Server &server, User &user, std::vector<std::string> &arguments);
/* Connection Registration */
		void pass(Server &server, User &user, std::vector<std::string> &arg);
		void nick(Server &server, User &user, std::vector<std::string> &arg);
		void user(Server &server, User &user, std::vector<std::string> &arg);
		void quit();
/* Channel operations */
		void join();
		void part();
		void topic();
		void names();
		void invite();
		void leave();
		void kick();
		void mode();
		void pmsg();
		void ping(); // void pong() ??
		void capls(Server &server, User &user, std::vector<std::string> &arg);
	private:
		std::map<std::string, cmdFPtr> cmdMap;
};
