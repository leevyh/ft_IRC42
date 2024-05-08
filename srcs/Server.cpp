#include "Server.hpp"
#include "IRC.hpp"
#include "User.hpp"
#include "unistd.h"
#include <algorithm>
#include <netdb.h>

Server::Server() {
	_port = 6667;
	_password = "default";
	_nb_of_users = 0;
	_networkname = "IRC_DE_LA_MORT";
}

Server::Server(long port, std::string password) : _port(port), _password(password) {
	_nb_of_users = 0;
	_networkname = "IRC_DE_LA_MORT";
}

Server::~Server() {
	for (size_t i = 0; i < _clientmap.size(); i++) {
		disconnect(_clientmap.begin()->second);
	}
	_clientmap.clear();
	for (std::vector<pollfd>::iterator it = _pollfdmap.begin(); it != _pollfdmap.end(); it++) {
		close(it->fd);
	}
	_pollfdmap.clear();
}

Server::Server(Server const &copy) {*this = copy;}

Server &Server::operator=(Server const &rhs) {
	if (this != &rhs) {
		_port = rhs._port;
		_nb_of_users = rhs._nb_of_users;
		_password = rhs._password;
		_networkname = rhs._networkname;
		_pollfdmap = rhs._pollfdmap;
		_clientmap = rhs._clientmap;
		_channels = rhs._channels;
	}
	return (*this);
}

/* ************************************************************************** */

void	Server::add_channelList(Channel &channel) {_channels.push_back(channel);}

void	Server::remove_channelList(Channel& channel) {
std::cout << "remove_channelList: " << channel.get_ChannelName() << std::endl;
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if (it->get_ChannelName() == channel.get_ChannelName()) {
			_channels.erase(it);
			return;
		}
	}
	return;
}

void	Server::delete_channelList(void) {_channels.clear();}

bool Server::is_onServer(std::string to_find) {
	for (std::map<int, User>::iterator it = this->get_clientmap().begin();
		it != this->get_clientmap().end(); ++it)
		if (it->second.get_nickname() == to_find)
			return (true);
	return (false);
}

/* ************************************************************************** */

long Server::get_Port(void) const {return (_port);}

std::string Server::get_Password(void) const {return (_password);}

std::string Server::get_networkname(void) const {return (_networkname);}

std::map<int, User> &Server::get_clientmap(void) {return (_clientmap);}

std::vector<Channel> &Server::get_channels(void) {return (_channels);}

/* ************************************************************************** */

#include <iomanip>

void Server::init_serv(void) {
	int server_socket;
	int yes = 1;

	/* INIT SERVER + ERR HANDLING*/
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
		throw except("Error creating socket");
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		throw except("Error Setsockopt");
	if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1)
		throw except("Error setting socket to non-blocking");

	/* ADD OPTION IN SERVER_ADDR STRUCT */
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(_port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;

	/* BINDING/LISTEN + ERR HANDLING */
	if (bind(server_socket, (struct sockaddr *) &_server_addr, sizeof(_server_addr)) == -1)
		throw except("Error binding socket");
	if (listen(server_socket, 10) == -1)
		throw except("Error listening on socket");

	/* SERVER START MESSAGE */
	std::cout << "\033[1J\033[1;1H\033[38;2;160;160;160m/******** SERVER STARTING ********\\" << std::endl << std::endl;
	std::cout << "  Server Listening on port: " << _port << std::endl;
	std::cout << "  Server Address listen: " << inet_ntoa(_server_addr.sin_addr) << std::endl;
	std::cout << "  Server Port: " << ntohs(_server_addr.sin_port) << std::endl;
	std::cout << "  Server Password: " << _password << std::endl << std::endl;
	std::cout << "\\********** SERVER LOG ***********/" << std::endl;

	/* ADD SERVER SOCKET TO POLLFD */
	_pollfdmap.push_back(pollfd());
	_pollfdmap.back().fd = server_socket;
	_pollfdmap.back().events = POLLIN;
}

void Server::start_serv(void) {
	if (poll(&_pollfdmap[0], _pollfdmap.size(), 2000) == -1) { // -1 means wait indefinitely, but we can change it to a timeout 1000000 for 1 second
		if (signal_value == false)
			throw except("poll exception");
	}
	if (_pollfdmap[0].revents == POLLIN)
		new_Connection_Client();
	else
		get_New_Client_Message();
}

void Server::new_Connection_Client(void) {
	struct sockaddr_in user_addr;
	socklen_t user_addr_len = sizeof(user_addr);
	int client_socket;


	/* ACCEPT CONNECTION + ERR HANDLING */
	client_socket = accept(_pollfdmap[0].fd, (struct sockaddr *) &user_addr, &user_addr_len);
	if (client_socket == -1)
		throw except("Error accepting connection");
	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
		throw except("Error setting connection to non-blocking");

	/* DISPLAY NEW CONNECTION */
	unsigned short port = ntohs(user_addr.sin_port);
	std::cout << std::endl << "	\033[38;2;100;160;100m New connection" << std::endl;
	std::cout << std::endl << "  New connection on socket: " << client_socket << std::endl;
	std::cout << "  User port: " << port << std::endl;
	std::cout << "  User IP: " << inet_ntoa(user_addr.sin_addr) << std::endl << std::endl;
	std::cout << "	End of connection" << std::endl << std::endl;
	std::cout << "\033[38;2;160;160;160m\\********************************/" << std::endl;

	/* CREATE NEW USER + ADD TO MAP + ADD TO POLLFD */
	User user(client_socket);
	user.set_ip(inet_ntoa(user_addr.sin_addr));
	user.set_port(port);
	_clientmap[client_socket] = user;
	_pollfdmap.push_back(pollfd());
	_pollfdmap.back().fd = client_socket;
	_pollfdmap.back().events = POLLIN;
	_nb_of_users++;
}

void Server::get_New_Client_Message(void) {
	if (_nb_of_users != 0 && _pollfdmap.size() != 1) {
		std::vector<pollfd>::iterator it;
		size_t i;
		int j = 0;
		for (i = 0; i < _pollfdmap.size(); i++) {
			j++;
			if (_pollfdmap[i].revents == POLLIN) {
				char buf[1024];
				ssize_t bytes = recv(_clientmap[_pollfdmap[i].fd].get_fd(), buf, 1024, 0);
				if (bytes <= 0) {
					memset(buf, 0, 1024);
					disconnect(_clientmap[_pollfdmap[i].fd]);
					return;
				}
				else {
					buf[bytes] = '\0';
					_clientmap[_pollfdmap[i].fd].joinBuffer(buf);
					_clientmap[_pollfdmap[i].fd].receive(*this);
					memset(buf, 0, 1024);
					if (_clientmap[_pollfdmap[i].fd].get_status() == false) {
						disconnect(_clientmap[_pollfdmap[i].fd]);
					}
				}
			}
		}
	}
	else {
		return;
	}
}

void Server::disconnect(User &user) {
	std::cout << std::endl << "	\033[38;2;160;100;100m New logout" << std::endl;

	std::cout << std::endl << "  New logout on fd: " << user.get_fd() << std::endl;
	std::cout << "  User port: " << user.get_port() << std::endl;
	std::cout << "  User IP: " << user.get_ip() << std::endl << std::endl;
	std::cout << "	End of connection" << std::endl << std::endl;
	std::cout << "\033[38;2;160;160;160m\\********************************/" << std::endl;
	std::vector<pollfd>::iterator it = std::find_if(_pollfdmap.begin(), _pollfdmap.end(), IsClientFDPredicate(user.get_fd()));
	if (it != _pollfdmap.end()) {
		_pollfdmap.erase(it);
	}
	close(user.get_fd());
	std::map<int, User>::iterator it_fd;
	it_fd = _clientmap.find(user.get_fd());
	if (it_fd != _clientmap.end()) {
		_clientmap.erase(it_fd);
	}
	_nb_of_users--;
	if (_nb_of_users < 1) {
		_clientmap.clear();
	}
}

/* ************************************************************************** */

void Server::sendMsg(User &user, std::string message, int code) const {
	std::string msg;
	switch (code) {
		case 1: // Message au User avec nom du server
			msg = ":" + this->get_networkname() + " " + message + "\r\n";
			if (send(user.get_fd(), msg.c_str(), msg.length(), 0) == -1)
				std::perror("send:");
			break;
		case 2: // Message au User sans nom du server
			msg = ":" + message + "\r\n";
			if (send(user.get_fd(), msg.c_str(), msg.length(), 0) == -1)
				std::perror("send:");
			break;
	}
}

