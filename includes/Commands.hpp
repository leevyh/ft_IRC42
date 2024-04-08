#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "Server.hpp"
#include "User.hpp"

class Server;
class User;
class Commands
{
	public:
		void getcommand(Server& server, User& user, std::vector<std::string>& argument);
/* Connection Registration */
		void pass();
		void nick(Server& server, User& user, std::vector<std::string>& nickname);
		void user();
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

	private:
		Commands( void );
		~Commands( void );
		typedef void (Commands::*cmdFPtr)(Server& server, User& user, std::vector<std::string>& argument);
		std::map<std::string, cmdFPtr> cmdMap;

};
