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
class Channel;
class Server {
	public:
		Server();
		Server(long port, std::string password);
		Server(Server const &copy);
		Server &operator=(Server const &rhs);
		~Server();
// GETTERS
		long get_Port(void) const;
		std::string get_Password(void) const;
		std::string get_networkname(void) const;
		std::vector<pollfd> get_fds(void) const;
		std::map<int, User> &get_clientmap();
// FUNCTIONS
		void init_serv(void);
		void start_serv(void);
		void new_Connection_Client(void);
		void get_New_Client_Message(void);
		void sendMsg(User &user, std::string message, int code) const;
		void disconnect(User &user);
		void timeout_disconnect(User &user);
		std::map<std::string, Channel> &get_channels(void);
		bool is_onServer(std::string to_find);
// FUNCTIONS BOT
		void init_bot(void);
	private:
		struct sockaddr_in _server_addr;
		long _port;
		int _nb_of_users;
		std::string _password;
		std::string _networkname;
		std::vector<pollfd> _pollfdmap;
		std::map<int, User> _clientmap;
		std::map<std::string, Channel> _channels;
};
