/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lazanett <lazanett@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 15:22:43 by lazanett          #+#    #+#             */
/*   Updated: 2024/05/01 16:30:20 by lazanett         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bot.hpp"
#include "../includes/IRC.hpp"
#include <sstream>

int	main(int ac, char **av)
{
	try {
		check_args_bonus(ac, av);
		Bot bot;
		bot.init_bot();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}


//==========================CONSTRUCTOR/DESTRUCTOR===========================//
Bot::Bot(void) {

	_signal_value = false;
	_port = 6667;
	_serverIP = "127.0.0.1";
	_nickname = "bot";
	_username = "bot";
}

Bot::~Bot(void) {
	
}

//===============================FUNCTION==================================//

void Bot::init_bot() {

	char buffer[1024];

	_clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_clientSocket == -1) {
		std::cerr << "Error creating socket" << std::endl;
		close(_clientSocket);
		exit(1);
	}
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(_port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	if (connect(_clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) { //Voir connection sur un poste distant
		std::cerr << "Error : the bot isn't connect" << std::endl;
		close(_clientSocket);
		exit(1);
	}

	send(_clientSocket, "NICK bot C3PO\r\n", 8, 0);
	send(_clientSocket, "PASS coucou\r\n", 8, 0);
	
	// //char *tmp = "PASS ";
	// char *tmp2 = strcat("PASS ",_pass);
	// const char *tmp3 = strcat(tmp2, "\r\n");
	// //char *temp = "NICK ";
	// char *temp2 = strcat("NICK ", _nickname);
	// const char *temp3 = strcat(temp2, "\r\n");
	// // char *t = "USER";
	// char *t2 = strcat("USER ", _username);
	// const char *t3 = strcat(t2, "\r\n");
	// send(_clientSocket, "CAP LS\r\n", 8, 0);
	// send(_clientSocket, tmp3, strlen(tmp3) , 0);
	// send(_clientSocket, temp3, strlen(temp3) , 0);
	// send(_clientSocket, t3, strlen(t3) , 0);
	// send(_clientSocket, "JOIN #test\r\n", 12 , 0);
	// send(_clientSocket, "PRIVMSG #test :bleu\r\n", 23 , 0);

	sleep(100);
	send(_clientSocket, "QUIT :leaving\r\n", 15 , 0);
	sleep(2);
	close(_clientSocket);// fermer la connexion
}

void Bot::check_args_bonus(int argc, char **argv) {
	if (argc == 3) {
		for (int i = 0; argv[1][i]; i++) {
			if (!isdigit(argv[1][i]))
				throw except("Port must be a number");
		}
		long port = strtol(argv[1], NULL, 10);
		if (port < 1024 || port > 65535)
			throw except("Port must be between 1024 and 65535");
	}
	else 
		throw except("Usage: ./ircserv <port> <password>");
	_pass = argv[3];
}

// void Server::start_serv(void) {
// 	if (poll(&_pollfdmap[0], _pollfdmap.size(), 2000) == -1)
// 	{
// 		if (signal_value == false) {
// 			std::cerr << "Signal" << std::endl;
// 			// throw exception("poll exception")
// 			exit(1);
// 		}
// 	}
// 	if (_pollfdmap[0].revents == POLLIN) {
// 		new_Connection_Client();
// 	}
// 	else {
// 		get_New_Client_Message();
// 	}
// }

// void Bot::sendMsg(Bot &bot, std::string message, int code) const {
// 	std::string msg;
// 	switch (code) {
// 		case 1:
// 			msg = ":" + this->get_networkname() + " " + message + "\r\n";
// 			if (send(bot.get_fd(), msg.c_str(), msg.length(), 0) == -1)
// 				std::perror("send:");
// 			break;
// 		case 2:
// 			msg = ":" + message + "\r\n";
// 			if (send(bot.get_fd(), msg.c_str(), msg.length(), 0) == -1)
// 				std::perror("send:");
// 			break;
// 	}
// }

void	Bot::info(void) {

	std::cout << "THIS IS A COMMAND LIST THAT YOU CAN USE ON IRC" << std::endl << std::endl;
	std::cout << "/nick + [new nickname] == rename nickname on the server " << std::endl;
	std::cout << "/msg + [nickname] + [MESSAGE] == send a message to someone" << std::endl;
	std::cout << "/part + [channel's name] + [message of what you're leaving](opt) == leave a channel" << std::endl;
	std::cout << "/join #[channel's name] == join a channel" << std::endl;
	std::cout << "/topic + [channel's topic desciption]" << std::endl;
	std::cout << "/invite + [nickname] == add a user in a channel" << std::endl<< std::endl;
	std::cout << "/kick + [nickname] == kick a channel user out" << std::endl;
	std::cout << "[MODE] == modify channel settings" << std::endl;
	std::cout << "/mode +l == add a limit of users" << std::endl;
	std::cout << "/mode -l == delete the limit" << std::endl;
	std::cout << "/mode +i == add the possibility of invite users" << std::endl;
	std::cout << "/mode -i == remove the invitation option " << std::endl;
	std::cout << "/mode +k == add a key for the channel" << std::endl;
	std::cout << "/mode -k == remove the key" << std::endl;
	std::cout << "/mode +o + [nickname] == give opertor rights for the channel" << std::endl;
	std::cout << "/mode -o + [nickname] == remove rights for the channel" << std::endl;
	std::cout << "/mode +t + [nickname] == channel's topic is accessible to channel's user " << std::endl;
	std::cout << "/mode -t + [nickname] == channel's topic is accessible only for channel's operator" << std::endl;
}

int	Validcommand(std::string command)
{
	if (command == "INFO")
		return 0;
	return 1;
}
void Bot::bot_command(std::string command) {

	void	(Bot::*funcPtr)();

	if (Validcommand(command) == 1) {
		std::cout << "Error : Try [DEBUG] or [INFO] or [WARNING] or [ERROR]" << std::endl;
		return;
	}
	switch (command[0]) {
		case 'I':
			funcPtr = &Bot::info;
			break;
		//qui est connecter sur le serveur
		//quels channels sont deja creer et qui ya dedans
	}
	(this->*funcPtr)();
}



//===================================GETTERS====================================//

// void Bot::set_nickname(char * &nickname) {
// 	this->_nickname = nickname;
// }