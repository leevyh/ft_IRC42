/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lazanett <lazanett@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 15:22:43 by lazanett          #+#    #+#             */
/*   Updated: 2024/05/03 17:32:50 by lazanett         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bot.hpp"
#include "../includes/IRC.hpp"
#include "../includes/Channel.hpp"
#include <sstream>

bool signal_value_bot = false;

int	main(int ac, char **av)
{
	try {
		check_args_bonus(ac, av);
		Bot bot(av);
		bot.init_bot(signal_value_bot);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}


//==========================CONSTRUCTOR/DESTRUCTOR===========================//
Bot::Bot() {}

Bot::Bot(char **av) {

	_signal_value = false;
	_port = 6667;
	_serverIP = "127.0.0.1";
	_nickname = "bot";
	_username = "bot";
	_pass = av[2];
}

Bot::~Bot(void) {}

//===============================FUNCTION==================================//

void Bot::init_bot(bool signal_value_bot) {

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
	send(_clientSocket, "CAP LS\r\n", 8, 0);
	std::string pass = "PASS " + _pass + "\r\n";
	send(_clientSocket, "NICK bot\r\n", 10, 0);
	send(_clientSocket, pass.c_str(), pass.length(), 0);
	send(_clientSocket, "USER bot bot localhost :bot bot\r\n", 33, 0);
	while (!signal_value_bot)
	{
		ssize_t bytesRead = recv(_clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesRead < 1) {
			signal_value_bot = true;
			break;
		}
		else {
			buffer[bytesRead] = '\0';
			std::string buffer_tmp;
			buffer_tmp.append(buffer);
			std::cout << "RECEPTION" << std::endl;
			std::cout << "LINE " << buffer_tmp;
			if (buffer_tmp.find("PRIVMSG"))
				send(_clientSocket, "PRIVMSG lazanett :J'ai bien recu ton message\r\n", 46, 0);
			// bot_command(command);
			memset(buffer, 0, 512);
		}
	}
	sleep(10);
	send(_clientSocket, "QUIT :leaving\r\n", 15 , 0);
	sleep(2);
	close(_clientSocket);// fermer la connexion
}

std::vector<std::string>	split_space(const std::string& str) {
	std::vector<std::string> channels_result;
	std::string delimiter = " ";
	char *args = strtok((char *)str.c_str(), " ");
	while (args != NULL) {
		channels_result.push_back(args);
		args = strtok(NULL, " ");
	}
	return (channels_result);
}

std::vector<std::string> split_arg(std::string buffer)
{
	std::vector<std::string> TabArgument;
	char *arg = strtok((char *)buffer.c_str(), "\r\n");
	while (arg == NULL && !buffer.empty())
	{
		TabArgument.push_back(arg);
		arg = strtok(NULL, "\r\n");
	}
	return TabArgument;
}




void check_args_bonus(int argc, char **argv) {
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
}

// void	Bot::connexion(Server &server, User &user, Channel chan, std::string channelname) {
// void	Bot::connexion(Server &server, User &user, Channel chan, std::string channelname) {

// 	int flag = 0;
// 	if (!server.get_channels().empty()) {
// 		for (std::vector<Channel>::iterator it = server.get_channels().begin(); \
// 			it != server.get_channels().end(); ++it) {
// 			if (channelname == it->get_ChannelName())
// 				flag = 1;
// 			else
// 				continue;
// 		}
// 		if (flag)
// 		{
// 			std::string msg = "Users connectes in the" + chan.get_ChannelName() + "channel :";
// 			for (std::vector<User>::iterator it = chan.get_ChannelUser().begin(); 
// 				it != chan.get_ChannelUser().end(); ++it) {
// 					msg += " " + it->get_username();
// 			}
// 			server.sendMsg(user, msg, 2);
// 		}
// 		server.sendMsg(user, ERR_NOSUCHCHANNEL(user, channelname), 2);
// 	}
// 	else
// 		server.sendMsg(user, "No channels on the Server", 2);
// }

// void	Bot::man(Server &server, User &user) {

// 	std::string msg = "THIS IS A COMMAND LIST THAT YOU CAN USE ON IRC\n\n";
// 	msg += "/nick + [new nickname] == rename nickname on the server\n";
// 	msg += "/msg + [nickname] + [MESSAGE] == send a message to someone\n";
// 	msg += "/part + [channel's name] + [message of what you're leaving](opt) == leave a channel\n";
// 	msg += "/join #[channel's name] == join a channel\n";
// 	msg += "/topic + [channel's topic desciption]\n";
// 	msg += "/invite + [nickname] == add a user in a channel\n\n";
// 	msg += "/kick + [nickname] == kick a channel user out\n";
// 	msg += "[MODE] == modify channel settings\n";
// 	msg += "/mode +l == add a limit of users\n";
// 	msg += "/mode -l == delete the limit\n";
// 	msg += "/mode +i == add the possibility of invite users\n";
// 	msg += "/mode -i == remove the invitation option\n";
// 	msg += "/mode +k == add a key for the channel\n";
// 	msg += "/mode -k == remove the key\n";
// 	msg += "/mode +o + [nickname] == give opertor rights for the channel\n";
// 	msg += "/mode -o + [nickname] == remove rights for the channel\n";
// 	msg += "/mode +t + [nickname] == channel's topic is accessible to channel's user\n";
// 	msg += "/mode -t + [nickname] == channel's topic is accessible only for channel's operator\n";
// 	server.sendMsg(user, msg, 2);
// }

// void Bot::list(Server &server, User &user)
// {
// 	if (!server.get_channels().empty()) {
// 		std::string msg = "Channels's list on the server :\n";
// 		for (std::vector<Channel>::iterator it = server.get_channels().begin(); \
// 			it != server.get_channels().end(); ++it) {
// 				msg += it->get_ChannelName() + " : ";
// 				std::vector<User> user_list = it->get_ChannelUser();
// 				for (std::vector<User>::iterator ita = user_list.begin(); ita != user_list.end(); ++ita) {
// 					msg += ita->get_username() + ".";
// 				}
// 				msg += "\n";
// 		}
// 		server.sendMsg(user, msg, 2);
// 	}
// 	else
// 	{
		
// 		server.sendMsg(user, "No channels on the Server", 2);
// 	}

// }

// int	Validcommand(std::string command)
// {
// 	if (command == "Man" || command ==  "Connexion" || command == "List" || command == "Server")
// 		return 0;
// 	return 1;
// }
// void Bot::bot_command(std::vector<std::string> command) {

// 	void	(Bot::*funcPtr)();

// 	if (Validcommand(command[2]) == 1) {
// 		std::cout << "Error : Command Unknow" << std::endl << "Try [Man] or [Connexion] or [List] or [Server]" << std::endl;
// 		return;
// 	}
// 	switch (command[2][0]) {
// 		case 'I': // INFO
// 			funcPtr = &Bot::man(&server, &user);
// 			break;
// 		case 'C': //CONNEXION + Channelsname
// 			if (command[3].empty()) {
// 				std::cout << "Error : required channel" << std::endl;
// 				funcPtr = NULL;
// 			}
// 			else {
// 				funcPtr = &Bot::connexion(&server,  &user, chan, command[3]);
// 			}
// 			break;
// 		case 'L'://LIST
// 			funcPtr = &Bot::list(&server, &user);
// 			break;
// 		case 'S'
// 		//qui est connecter sur le serveur
// 		//quels channels sont deja creer
// 	}
// 	(this->*funcPtr)();
// }



//===================================GETTERS====================================//

// void Bot::set_nickname(char * &nickname) {
// 	this->_nickname = nickname;
// }