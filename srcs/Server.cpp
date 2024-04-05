#include "Server.hpp"
#include "IRC.hpp"
#include "User.hpp"

Server::Server()
{
	_port = 6667;
	_password = "default";
}

Server::Server(long port, std::string password) : _port(port), _password(password)
{
	std::cout << "Server created with port: " << _port << " and password: " << _password << std::endl;
}

Server::Server(Server const & copy)
{
	*this = copy;
}

Server & Server::operator=(Server const & rhs)
{
	if (this != &rhs)
	{
		_port = rhs._port;
		_password = rhs._password;
	}
	return (*this);
}

long	Server::get_Port(void) const { return (_port); }
std::string	Server::get_Password(void) const { return (_password); }

void	Server::init_serv(void)
{
	int	server_socket;

	// char	msg[1024];

	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(_port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (server_socket == -1)
	{
		std::cerr << "Error creating socket" << std::endl;
		//throw exception("Error creating socket")
		exit(1);
	}
	//setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error setting socket to non-blocking" << std::endl;
		//throw exception("Error setting socket to non-blocking")
		exit(1);
	}

	//inet_ntop ?

	if (bind(server_socket, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) == -1)
	{
		std::cerr << "Error binding socket" << std::endl;
		//throw exception("Error binding socket")
		exit(1);
	}

	if (listen(server_socket, 200) == -1)
	{
		std::cerr << "Error listening on socket" << std::endl;
		//throw exception("Error listening on socket")
		exit(1);
	}

	std::cout << "End of init_serv" << std::endl;
	std::cout << "Server listening on port " << _port << std::endl;
	std::cout << "Socket: " << server_socket << std::endl;
	std::cout << "Server address: " << inet_ntoa(_server_addr.sin_addr) << std::endl;
	std::cout << "Server port: " << ntohs(_server_addr.sin_port) << std::endl;
	_fds.push_back(pollfd());
	_fds.back().fd = server_socket;
	_fds.back().events = POLLIN;
}

void Server::start_serv(void)
{
	if (poll(&_fds[0], _fds.size(), 10000) == -1) // -1 means wait indefinitely but we can change it to a timeout 1000000 for 1 second
	{
		if (signal_value == false)
		{
			std::cerr << "Signal" << std::endl;
			// throw exception("poll exception")
			exit(1);
		}
	}
	if (_fds[0].revents == POLLIN)
		new_Connection_Client();
	else
	{
		sleep(0.2);
		// std::cout << "No new connection" << std::endl;
	}
}


void	Server::new_Connection_Client(void)
{
	socklen_t user_addr_len = sizeof(_user_addr);

	int client_socket = accept(_fds[0].fd, (struct sockaddr *)&_user_addr, &user_addr_len);

	if (client_socket == -1)
	{
		std::cerr << "Error accepting connection" << std::endl;
		// throw exception("Error accepting connection")
		exit(1);
	}
	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error setting connection to non-blocking" << std::endl;
		// throw exception("Error setting connection to non-blocking")
		exit(1);
	}
	std::cout << "New connection on client_socket: " << client_socket << std::endl;
	std::cout << "User address: " << inet_ntoa(_user_addr.sin_addr) << std::endl;
	std::cout << "User port: " << ntohs(_user_addr.sin_port) << std::endl;
	std::cout << "User test: " << _user_addr.sin_family << std::endl;



	User user();
	_fds.push_back(pollfd());
	_fds.back().fd = client_socket;
	_fds.back().events = POLLIN;
}

Server::~Server()
{
}
