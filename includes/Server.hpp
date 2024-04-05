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
#include <poll.h>
#include "IRC.hpp"

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
		std::vector <pollfd>	get_fds(void) const;
		void	start_serv(void);
		void	new_Connection_Client(void);

	private:
		long		_port;
		std::string	_password;
		struct sockaddr_in _user_addr;
		struct	sockaddr_in _server_addr;

		std::vector <pollfd>	_fds;
};
