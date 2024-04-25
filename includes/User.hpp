#pragma once

#include <iostream>
#include <fstream>
#include <unistd.h>
#include "IRC.hpp"

class Server;
class Commands;
class User {
	private:
		std::string _nickname;
		std::string _username;
		std::string _realname;
		std::string _password;
		std::string _ip;
		bool _status;				// connected or not (/connect)
		bool _authenticated;		// authenticated or not (PASS + NICK + USER)
		int _fd;
		std::string _buffer;
		int _lastping;
	public:
		User(void);
		User(const int fd);
		User(const User &src);
		User &operator=(const User &rhs);
		~User(void);
// SETTERS
		void set_nickname(const std::string &nickname);
		void set_username(const std::string &username);
		void set_realname(const std::string &realname);
		void set_password(const std::string &password);
		void set_ip(const std::string &ip);
		void set_status(bool status);
		void set_authenticated(bool status);
		void set_lastping(const int &ping);
// GETTERS
		const std::string &get_nickname(void) const;
		const std::string &get_username(void) const;
		const std::string &get_realname(void) const;
		const std::string &get_password(void) const;
		const std::string &get_ip(void) const;
		bool get_status(void) const;
		bool get_authenticated(void) const;
		int get_fd(void) const;
// FUNCTIONS
		void joinBuffer(const char *buffer);
		void receive(Server &server);
		void parseClientMessage(Server &server, std::string line);
		void authentication(Server &server, Commands &cmd, std::vector<std::string> arg);
};
std::ostream &operator<<(std::ostream &o, const User &src);

std::vector<std::string> splitcmd(std::string line);
