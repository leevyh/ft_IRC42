#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include <poll.h>
#include "IRC.hpp"

class User;
class Commands;
class Server {
	public:
		Server();
		Server(long port, std::string password);
		Server(Server const &copy);
		Server &operator=(Server const &rhs);
		~Server();
		void init_serv(void);
		long get_Port(void) const;
		std::string get_Password(void) const;
		std::string get_networkname(void) const;
		std::vector<pollfd> get_fds(void) const;
		void start_serv(void);
		void new_Connection_Client(void);
		void get_New_Client_Message(void);
		std::map<std::string, User *> get_usersbynick(void) const;
		void sendMsg(User &user, std::string message) const;
		void disconnect(User &user);
	private:
		struct sockaddr_in _server_addr;
		long _port;
		int _nb_of_users;
		std::string _password;
		std::string _networkname;
		std::vector<pollfd> _pollfdmap;
		std::map<int, User> _clientmap;          // users by fds
		std::map<std::string, User *> _users; // users by nickname
};
