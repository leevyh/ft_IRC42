#include "Server.hpp"
#include "IRC.hpp"
#include "User.hpp"
#include "unistd.h"
#include <algorithm>

Server::Server() {
	_port = 6667;
	_password = "default";
	_nb_of_users = 0;
	_networkname = "IRC_DE_LA_MORT";
	// _irssi = false;
}

Server::Server(long port, std::string password) : _port(port), _password(password) {
	std::cout << "Server created with port: " << _port << " and password: " << _password << std::endl;
	_nb_of_users = 0;
	_networkname = "IRC_DE_LA_MORT";
	// _irssi = false;
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
		_password = rhs._password;
		// _irssi = false;
		_channels = rhs._channels;
	}
	return (*this);
}

/* ************************************************************************** */

long Server::get_Port(void) const {return (_port);}

std::string Server::get_Password(void) const {return (_password);}

std::string Server::get_networkname(void) const {return (_networkname);}

std::map<int, User> &Server::get_clientmap(void) {return (_clientmap);}


std::map<std::string, Channel> &Server::get_channels(void) {return (_channels);}

bool Server::get_Irssi(void) {return (_irssi);}

/* ************************************************************************** */

void Server::set_Irssi(bool status) {this->_irssi = status;}

/* ************************************************************************** */

void Server::init_serv(void) {
	int server_socket;
	int yes = 1;
	// char ip_addr[INET_ADDRSTRLEN];

	// char	msg[1024];

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		std::cerr << "Error creating socket" << std::endl;
		// throw exception("Error creating socket")
		exit(1);
	}
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		std::cerr << "Error Setsockopt" << std::endl;
	}
	if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error setting socket to non-blocking" << std::endl;
		// throw exception("Error setting socket to non-blocking")
		exit(1);
	}
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(_port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;

	// inet_ntop(AF_INET, &(_server_addr.sin_addr), ip_addr, INET_ADDRSTRLEN);

	if (bind(server_socket, (struct sockaddr *) &_server_addr, sizeof(_server_addr)) == -1) {
		std::cerr << "Error binding socket" << std::endl;
		// throw exception("Error binding socket")
		exit(1);
	}
	if (listen(server_socket, 10) == -1) {
		std::cerr << "Error listening on socket" << std::endl;
		// throw exception("Error listening on socket")
		exit(1);
	}
	std::cout << "Server listening on port " << _port << std::endl;
	std::cout << "Server address: " << inet_ntoa(_server_addr.sin_addr) << std::endl;
	std::cout << "Server port: " << ntohs(_server_addr.sin_port) << std::endl;
	_pollfdmap.push_back(pollfd());
	_pollfdmap.back().fd = server_socket;
	_pollfdmap.back().events = POLLIN;
}

void Server::start_serv(void) {
	if (poll(&_pollfdmap[0], _pollfdmap.size(), 2000) == -1) // -1 means wait indefinitely, but we can change it to a timeout 1000000 for 1 second
	{
		if (signal_value == false) {
			std::cerr << "Signal" << std::endl;
			// throw exception("poll exception")
			exit(1);
		}
	}
	if (_pollfdmap[0].revents == POLLIN) {
		new_Connection_Client();
	}
	else {
		get_New_Client_Message();
	}
}

void Server::new_Connection_Client(void) {
	struct sockaddr_in user_addr;
	socklen_t user_addr_len = sizeof(user_addr);
	std::cout << "Socket server juste avant le fcntl :" << _pollfdmap[0].fd << std::endl;
	int client_socket;
//	std::cout << listen(_pollfdmap[0].fd, 10) << std::endl;
	client_socket = accept(_pollfdmap[0].fd, (struct sockaddr *) &user_addr, &user_addr_len);
	if (client_socket == -1) {
		std::cerr << "Error accepting connection" << std::endl;
		// throw exception("Error accepting connection")
		exit(1);
	}
	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error setting connection to non-blocking" << std::endl;
		// throw exception("Error setting connection to non-blocking")
		exit(1);
	}
	std::cout << "New connection on client_socket: " << client_socket << std::endl;
	std::cout << "User address: " << inet_ntoa(user_addr.sin_addr) << std::endl;
	std::cout << "User port: " << ntohs(user_addr.sin_port) << std::endl;
	User user(client_socket);
	//set ip for user
	_clientmap[client_socket] = user;
	_pollfdmap.push_back(pollfd());
	_pollfdmap.back().fd = client_socket;
	_pollfdmap.back().events = POLLIN;
	_nb_of_users++;
	std::cout << "Number of users: " << _nb_of_users << std::endl;
}

//void Server::get_New_Client_Message(void) {
//	if (_nb_of_users != 0 && _pollfdmap.size() != 1) {
//		std::vector<pollfd>::iterator it;
//		int j = 0;
//		for (it = _pollfdmap.begin(); it->fd != _pollfdmap.end()->fd; it++) {
//			std::cout << j << " : " << it->fd << std::endl;
//			std::cout << "Adresse pollfdend : " << _pollfdmap.end()->fd << std::endl;
//			j++;
//			if (it->revents == POLLIN) {
//				char buf[1024];
//				int bytes = recv(_clientmap[it->fd].get_fd(), buf, 1024, 0);
//				std::cout <<"bytes :" << bytes << std::endl;
//				if (bytes <= 0) {
//					memset(buf, 0, 1024);
//					disconnect(_clientmap[it->fd]);
//					std::cout << "deco" << std::endl;
//					return;
//				}
//				else {
//					buf[bytes] = '\0';
//					_clientmap[it->fd].joinBuffer(buf);
//					_clientmap[it->fd].receive(*this);
//					memset(buf, 0, 1024);
//					if (_clientmap[it->fd].get_status() == false) {
//						disconnect(_clientmap[it->fd]);
//					}
//				}
//			}
//		}
//	}
//	else {
//		return;
//	}
//}








void Server::get_New_Client_Message(void) {
	if (_nb_of_users != 0 && _pollfdmap.size() != 1) {
		std::vector<pollfd>::iterator it;
		size_t i;
		int j = 0;
		for (i = 0; i < _pollfdmap.size(); i++) {
//			std::cout << j << " : " << _pollfdmap[i].fd << std::endl;
//			std::cout << "Adresse pollfdend : " << _pollfdmap[_pollfdmap.size() - 1].fd << std::endl;
			j++;
			if (_pollfdmap[i].revents == POLLIN) {
				char buf[1024];
//				int bytes = recv(_clientmap[it->fd].get_fd(), buf, 1024, 0);
				ssize_t bytes = recv(_clientmap[_pollfdmap[i].fd].get_fd(), buf, 1024, 0);
//				std::cout <<"bytes :" << bytes << std::endl;
//				std::cout << "buffer : " << buf << std::endl;
				if (bytes <= 0) {
					memset(buf, 0, 1024);
					disconnect(_clientmap[_pollfdmap[i].fd]);
					std::cout << "deco" << std::endl;
					return;
				}
				else {
					buf[bytes] = '\0';
					_clientmap[_pollfdmap[i].fd].joinBuffer(buf);
					_clientmap[_pollfdmap[i].fd].receive(*this);
					memset(buf, 0, 1024);
					if (_clientmap[_pollfdmap[i].fd].get_status() == false) {
						std::cout << "je passe ici ?" << std::endl;
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


//void	Server::send_MsgToUsers(Server &server, User &sender, User &dest)











void Server::disconnect(User &user) {
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

//	std::vector<pollfd>::iterator it = std::find_if(_pollfdmap.begin(), _pollfdmap.end(), IsClientFDPredicate(user.get_fd()));
//	if (it != _pollfdmap.end()){
//		_pollfdmap.erase(it);
//	}
//	close(user.get_fd());
//	std::map<int, User>::iterator it_fd = _clientmap.find(user.get_fd());
//	if (it_fd != _clientmap.end()) {
//		_clientmap.erase(it_fd);
//	}
//	_nb_of_users--;
//	if (_nb_of_users < 1) {
//		_clientmap.clear();
//	}
//}

void Server::sendMsg(User &user, std::string message, int code) const {
	std::string msg;
	switch (code) {
		case 1:
			msg = ":" + this->get_networkname() + " " + message + "\r\n";
			if (send(user.get_fd(), msg.c_str(), msg.length(), 0) == -1)
				std::perror("send:");
			break;
		case 2:
			msg = ":" + message + "\r\n";
			if (send(user.get_fd(), msg.c_str(), msg.length(), 0) == -1)
				std::perror("send:");
			break;
	}
}


