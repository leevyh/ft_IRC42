/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lazanett <lazanett@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 15:22:43 by lazanett          #+#    #+#             */
/*   Updated: 2024/04/30 17:02:53 by lazanett         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bot.hpp"

int	main(int ac, char **av)
{
	if (ac != 2)
		return 1;
	Bot bot;
	bot.init_bot();
	return 0;
}


//==========================CONSTRUCTOR/DESTRUCTOR===========================//
Bot::Bot(void) {

	_signal_value = false;
	_port = 6667;
	_serverIP = "127.0.0.1";
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
	if (connect(_clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cerr << "Error : the bot isn't connect" << std::endl;
		close(_clientSocket);
		exit(1);
	}
	// send(_clientSocket, "bot connected", 14 , 0);
	
	send(_clientSocket, "CAP LS\r\n", 8, 0);
	send(_clientSocket, "PASS coucou\r\n", 13 , 0);
	send(_clientSocket, "NICK bot\r\n", 10 , 0);
	send(_clientSocket, "USER bot bot localhost :Bot Bot\r\n", 33 , 0);
	send(_clientSocket, "bot connected\r\n", 15 , 0);
	sleep(2);
	send(_clientSocket, "QUIT :leaving\r\n", 15 , 0);
	sleep(2);
	close(_clientSocket);// fermer la connexion
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
	
	}
	(this->*funcPtr)();
}



//===================================GETTERS====================================//

void Bot::set_nickname(const std::string &nickname) {
	this->_nickname = nickname;
}