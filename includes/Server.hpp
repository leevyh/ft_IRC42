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
#include "User.hpp"

class User;

class Server
{
	public:
		Server();
		Server(long	port, std::string password);
		Server(Server const & copy);
		Server & operator=(Server const & rhs);
		~Server();

		void	init_serv(void);
		long		get_Port(void) const;
		std::string	get_Password(void) const;
		std::vector<pollfd>	get_fds(void) const;
		void	start_serv(void);
		void	new_Connection_Client(void);
		std::map<std::string, User*> get_usersbynick(void) const;
		void sendMsg(Server& server, User& user, std::string message) const;

	private:
		long		_port;
		std::string	_password;
		std::vector<pollfd>	_fds;
		std::map<std::string, User*>	_users;		// users by nickname
		std::map<int, User*>	_fdusers;			// users by fds
};
