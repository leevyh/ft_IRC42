#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 1024;
const int PORT = 6667;

int	main(void)
{
	int	server_socket;
	int	client_socket;

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	char	message[BUF_SIZE];

	//socket return a fd, PF_INET is protocol family IPv4, SOCK of SOCK_STREAM is a type of Sock and stream means it will be a stream.
	server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
	{
		std::cout << "socket error" << std::endl;
		return (1);
	}
	std::cout << "server socket created " << server_socket << std::endl;
	server_addr.sin_family = AF_INET; // -> PF_INET -> AF_INET
	std::cout << "server_addr.sin_family: " << server_addr.sin_family << std::endl;
	server_addr.sin_port = htons(PORT); // htons is host to network short, it converts the port number to network byte order 6789 -> 0x1A85
	std::cout << "server_addr.sin_port in hex: " << std::hex << server_addr.sin_port << std::endl;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY is to accept any incoming connection
	std::cout << "server_addr.sin_addr.s_addr: " << server_addr.sin_addr.s_addr << std::endl;
	bind(server_socket,(struct sockaddr*)&server_addr, sizeof(server_addr));
	std::cout << "binded" << std::endl;

	if (listen(server_socket, 3) == -1)
	{
		std::cout << "listen error" << std::endl;
		close(server_socket);
		return (1);
	}
	std::cout << "listening on port " << PORT << std::endl;

	socklen_t client_addr_size = sizeof(client_addr); //socklen_t is a type of unsigned int
	std::cout << "client_addr_size: " << client_addr_size << std::endl;
	client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
	if (client_socket == -1)
	{
		std::cout << "accept error" << std::endl;
		close(server_socket);
		return (1);
	}

	std::string welcome_message = "001 Welcome to the server!\r\n";
	if (send(client_socket, welcome_message.c_str(), welcome_message.size(), 0) == -1)
	{
		std::cout << "send error" << std::endl;
		close(client_socket);
		close(server_socket);
		return (1);
	}



	ssize_t bytes_received; // ssize_t is a type of signed int
	while ((bytes_received = recv(client_socket, message, BUF_SIZE, 0)) > 0) //recv like read
	{
		if (bytes_received == -1)
		{
			std::cout << "recv error" << std::endl;
			break;
		}
		message[bytes_received] = '\0';
		std::cout << "received: " << message << std::endl;
		send(client_socket, message, bytes_received, 0); //like write
	}

	close(client_socket);
	close(server_socket);
	return (0);
}
