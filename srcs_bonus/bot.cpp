/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lazanett <lazanett@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 15:22:43 by lazanett          #+#    #+#             */
/*   Updated: 2024/05/07 18:27:08 by lazanett         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bot.hpp"
#include "../includes/IRC.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>

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
	_flag_chifoumi = 0;
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
			// std::cout << "LINE = " <<  buffer_tmp << std::endl;
			std::size_t found = buffer_tmp.find("PRIVMSG");
			if (found != std::string::npos)
			{
				std::vector<std::string> argument = split_space(buffer_tmp);
				if (!argument.empty()) {
					std::vector<std::string> pre_requestor = split_pre_requestor(argument[0]);
					if (!pre_requestor.empty() || !pre_requestor[0].empty()) {
						std::vector<std::string> requestor = split_requestor(pre_requestor[0]);
						if (!requestor.empty() || !requestor[0].empty()) {
							_requestor = requestor[0];
							_msg = argument[3].substr(1);
							if (argument.size() > 3)
							{
								for (size_t i = 4; i < argument.size(); ++i) {
									_msg += " " + argument[i];
								}
							}
							//std::cout << "msg = " << _msg << std::endl;
							bot_command(_msg);
						}
					}
				}
			}
			// 	send(_clientSocket, "PRIVMSG lazanett :J'ai bien recu ton message\r\n", 46, 0);
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
	std::string del = " ";
	char *args = strtok((char *)str.c_str(), " ");
	while (args != NULL) {
		channels_result.push_back(args);
		args = strtok(NULL, " ");
	}
	return (channels_result);
}

std::vector<std::string>	split_pre_requestor(const std::string& str) {
	std::vector<std::string> channels_result;
	std::string del = ":";
	char *args = strtok((char *)str.c_str(), ":");
	while (args != NULL) {
		channels_result.push_back(args);
		args = strtok(NULL, ":");
	}
	return (channels_result);
}

std::vector<std::string>	split_requestor(const std::string& str) {
	std::vector<std::string> channels_result;
	std::string del = "!";
	char *args = strtok((char *)str.c_str(), "!");
	while (args != NULL) {
		channels_result.push_back(args);
		args = strtok(NULL, "!");
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

void sendPrivMsg(int clientSocket, const std::string& recipient, const std::string& message) {
	std::string pv_msg = "PRIVMSG " + recipient + " :" + message + "\r\n";
	send(clientSocket, pv_msg.c_str(), pv_msg.length(), 0);
}

void	Bot::man() {

	std::vector<std::string> lines = {
	"THIS IS A COMMAND LIST THAT YOU CAN USE ON IRC",
	"/nick + [new nickname] == rename nickname on the server",
	"/msg + [nickname] + [MESSAGE] == send a message to someone",
	"/part + [channel's name] + [message of what you're leaving](opt) == leave a channel",
	"/join #[channel's name] == join a channel",
	"/topic + [channel's topic desciption]",
	"/invite + [nickname] == add a user in a channel",
	"/kick + [nickname] == kick a channel user out",
	"[MODE] == modify channel settings",
	"/mode +l == add a limit of users",
	"/mode -l == delete the limit",
	"/mode +i == add the possibility of invite users",
	"/mode -i == remove the invitation option",
	"/mode +k == add a key for the channel",
	"/mode -k == remove the key",
	"/mode +o + [nickname] == give operator rights for the channel",
	"/mode -o + [nickname] == remove rights for the channel",
	"/mode +t + [nickname] == channel's topic is accessible to channel's user",
	"/mode -t + [nickname] == channel's topic is accessible only for channel's operator",
	};

	for (const std::string& line : lines) {
		sendPrivMsg(_clientSocket, _requestor, line);
	}
}

int	Bot::Validcommand(std::vector<std::string> arg)
{
	if (!arg.empty())
	{
		if (arg[0] == "Man\r\n" || arg[0] ==  "Chifoumi")
			return 0;
		return 1;
	}
	return 1;
}
void Bot::bot_command(std::string command) {

	if (!command.empty())
	{
		std::vector<std::string> args = split_space(command); 
		// for (size_t i = 0; i < args.size(); i++)
		// {
		// 	std::cout << i << args[i] << std::endl;
		// }
		if (Validcommand(args) == 1) {
			std::string error = "PRIVMSG " + _requestor + " :" + "Error : Command Unknow || Try [Man] or [Chifoumi] or [List] or [Server]\r\n";
			send(_clientSocket, error.c_str(), error.length(), 0);
			return;
		}
		switch (args[0][0]) {
			case 'M':
				man();
				break;
			case 'C':
				if (!args.empty() && args.size() == 2)
					chifoumi(args);
				else
				{
					std::string msg = "PRIVMSG " + _requestor + " :" + "Error : [Chifoumi] need more parameter\r\n";
					send(_clientSocket, msg.c_str(), msg.length(), 0);
				}
				break;
		}
	}
}

void	Bot::chifoumi(std::vector<std::string> arg)
{
	_flag_chifoumi = 1;
	if (arg[1] == "paper\r\n" || arg[1] == "scissors\r\n" || arg[1] == "rock\r\n")
	{
		std::string msg = "PRIVMSG " + _requestor + " :" + "bot is choosing...\r\n";
		send(_clientSocket, msg.c_str(), msg.length(), 0);
		std::this_thread::sleep_for(std::chrono::seconds(2));
		std::string msg1 = "PRIVMSG " + _requestor + " :" + "CHI...\r\n";
		send(_clientSocket, msg1.c_str(), msg1.length(), 0);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::string msg2 = "PRIVMSG " + _requestor + " :" + "FOU...\r\n";
		send(_clientSocket, msg2.c_str(), msg2.length(), 0);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::string msg3 = "PRIVMSG " + _requestor + " :" + "MI...\r\n";
		send(_clientSocket, msg3.c_str(), msg3.length(), 0);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		_choice_bot = choice() + "\r\n";
		srand(time(0));
		advantage(arg, _choice_bot);
		if (_n_arg == _n_choice_bot)
		{
			std::string msg = "PRIVMSG " + _requestor + " :" + "EQUALITY : both of you choose " + _choice_bot + "\r\n" ;
			send(_clientSocket, msg.c_str(), msg.length(), 0);
		}
		else if (_n_arg < _n_choice_bot)
		{
			std::string msg = "PRIVMSG " + _requestor + " :" + "LOSER : bot had chosen " + _choice_bot + "\r\n";
			send(_clientSocket, msg.c_str(), msg.length(), 0);
		}
		else if (_n_arg > _n_choice_bot)
		{
			std::string msg = "PRIVMSG " + _requestor + " :" + "VICTORY : bot had chosen " + _choice_bot + "\r\n";
			send(_clientSocket, msg.c_str(), msg.length(), 0);
		}
	}
	else
	{
		std::string msg = "PRIVMSG " + _requestor + " :" + "Error : need a valid parameter ; Try [rock] or [paper] or [scissors]\r\n";
		send(_clientSocket, msg.c_str(), msg.length(), 0);
	}
	
}

void	Bot::advantage(std::vector<std::string> arg, std::string choice_bot)
{
	_n_arg = 0;
	_n_choice_bot = 0;
	if (arg[1] == _choice_bot)
	{
		_n_arg = 1;
		_n_choice_bot = 1;
	}
	else if (arg[1] != _choice_bot && (((arg[1] == "paper\r\n") && (_choice_bot == "scissors\r\n")) || ((arg[1] == "scissors\r\n") && (_choice_bot == "rock\r\n")) || ((arg[1] == "rock\r\n") && (_choice_bot == "paper\r\n"))))
		_n_choice_bot = 1;
	else if (arg[1] != _choice_bot && (((arg[1] == "scissors\r\n") && (_choice_bot == "paper\r\n")) || ((arg[1] == "rock\r\n") && (_choice_bot == "scissors\r\n")) || ((arg[1] == "paper\r\n") && (_choice_bot == "rock\r\n"))))
		_n_arg = 1;
}

std::string choice() {

	std::vector<std::string> op = {"paper", "scissors", "rock"};
	random_tab(op);
	return (op[0]);
}

void	random_tab(std::vector<std::string>& tab) {
	
	int n = tab.size();
	for (int i = n - 1; i > 0; --i) {
		int j = rand() % (i + 1);
		std::string temp = tab[i];
		tab[i] = tab[j];
		tab[j] = temp;
	}
}


//===================================GETTERS====================================//

// void Bot::set_nickname(char * &nickname) {
// 	this->_nickname = nickname;
// }