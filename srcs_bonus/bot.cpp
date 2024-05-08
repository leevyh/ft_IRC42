/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lazanett <lazanett@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 15:22:43 by lazanett          #+#    #+#             */
/*   Updated: 2024/05/08 17:32:57 by lazanett         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bot.hpp"

bool signal_value_bot = false;
int	client_socket_global = 0;

int	main(int ac, char **av)
{
	try {
		check_args_bonus(ac, av);
		// std::signal(SIGINT, &signal_send);
		Bot bot(av);
		bot.init_bot();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

//=========================================CONSTRUCTOR/DESTRUCTOR=================================================//
Bot::Bot() {}

Bot::Bot(char **av) {

	// _signal_value_bot = false;
	_port = 6667;
	_serverIP = "127.0.0.1";
	_nickname = "bot";
	_username = "bot";
	_pass = av[2];
	_flag_num = 0;
	_it = 0;
	_it_left = 10;
	_t = 0;
}

Bot::~Bot(void) {}

//=================================================FUNCTION=====================================================//

int	Bot::connexion()
{
	_clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	client_socket_global = _clientSocket;
	if (_clientSocket == -1) {
		std::cerr << "Error creating socket" << std::endl;
		close(_clientSocket);
		return 1;
	}
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(_port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	if (connect(_clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cerr << "Error : the bot isn't connect" << std::endl;
		close(_clientSocket);
		return 1;
	}
	send(_clientSocket, "CAP LS\r\n", 8, 0);
	std::string pass = "PASS " + _pass + "\r\n";
	send(_clientSocket, "NICK bot\r\n", 10, 0);
	send(_clientSocket, pass.c_str(), pass.length(), 0);
	send(_clientSocket, "USER bot bot localhost :bot bot\r\n", 33, 0);
	return 0;
}

void Bot::init_bot() {

	char buffer[1024];

	if (connexion() == 0)
	{
		if (std::signal(SIGINT, signal_send) == SIG_ERR){};
		// try
		while (!(signal_value_bot))
		{
			std::signal(SIGINT, &signal_send);
			ssize_t bytesRead = recv(_clientSocket, buffer, sizeof(buffer) - 1, 0);
			std::cout << "BytesRead :" << bytesRead << std::endl;
			if (bytesRead < 1) {
				memset(buffer, 0, 512);
				signal_value_bot = true;
				break;
			}
			else {
				buffer[bytesRead] = '\0';
				std::string buffer_tmp;
				buffer_tmp.append(buffer);
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
								if (_flag_num)
									gessnum(_msg);
								else
									bot_command(_msg);
								
							}
						}
					}
				}
				memset(buffer, 0, 512);
			}
		}
	}
	else
		exit(0);
}

//==============================================COMMANDS========================================================//

int	Bot::Validcommand(std::vector<std::string> arg)
{
	if (!arg.empty())
	{
		if (arg[0] == "Man\r\n" || arg[0] ==  "Chifoumi" || arg[0] == "Number\r\n")
			return 0;
		return 1;
	}
	return 1;
}
void Bot::bot_command(std::string command) {

	if (!command.empty())
	{
		std::vector<std::string> args = split_space(command); 
		if (Validcommand(args) == 1) {
			std::string error = "PRIVMSG " + _requestor + " :" + "Error : try [Man] or [Chifoumi + option] or [Number]\r\n";
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
			case 'N':
				_flag_num = 1;
				get_random_num();
				_it = 0;
				_t = 0;
				break;
		}
	}
}

//===========================================MAN===========================================================//

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

void sendPrivMsg(int clientSocket, const std::string& recipient, const std::string& message) {
	std::string pv_msg = "PRIVMSG " + recipient + " :" + message + "\r\n";
	send(clientSocket, pv_msg.c_str(), pv_msg.length(), 0);
}

//===============================================NUMBER===========================================================//

int	Bot::legit_num(std::vector<std::string> arg)
{
	_num = arg[0];
	while (_num.size() >= 2 && _num.substr(_num.size() - 2) == "\r\n") {
		_num.erase(_num.size() - 2);
	}
	for (char c : _num) {
		if (!std::isdigit(c))
			return 0;
	}
	if (!(ft_atoi(_num) >= 0 && ft_atoi(_num) <= 1000 && _num.size() <= 4))
		return 0;
	return 1;
}

void Bot::gessnum(std::string command)
{
	std::vector<std::string> args = split_space(command);
	if (!args.empty() && args.size() == 1)
	{
		if (args[0] == "Stop\r\n")
		{
			_flag_num = 0;
			_it = 0;
			_it_left = 10;
			_t = 0;
			std::string msg = "PRIVMSG " + _requestor + " :" + "Bye Bye \r\n";
			send(_clientSocket, msg.c_str(), msg.length(), 0);
		}
		else if (legit_num(args) == 1)
			number();
		else if (args[0] == "Tips\r\n")
		{
			tips();
			_t++;
		}
	}
}

void	Bot::number()
{
	if (ft_atoi(_num) == _num_bot && _it_left != 0)
	{
		_flag_num = 0;
		_it = 0;
		_it_left = 10;
		_t = 0;
		auto n = std::to_string(_num_bot);
		std::string msg = "PRIVMSG " + _requestor + " :" + "CONGRATULATIONS: the bot's number was " + n + "\r\n";
		send(_clientSocket, msg.c_str(), msg.length(), 0);
	}
	else if (ft_atoi(_num) != _num_bot && _it_left != 0)
	{
		_it++;
		_it_left = 10 - _it;
		
		if (_it_left == 0)
		{
			_flag_num = 0;
			_it = 0;
			_it_left = 10;
			_t = 0;
			auto n = std::to_string(_num_bot);
			std::string msg = "PRIVMSG " + _requestor + " :" + "You can't try again, the number was " + n + ". Bye bye\r\n";
			send(_clientSocket, msg.c_str(), msg.length(), 0);
		}
		else
		{
			std::string clue;
			if (ft_atoi(_num) > _num_bot)
				clue = " the number is lower, ";
			else
				clue = " the number is upper, ";
			auto left = std::to_string(_it_left);
			std::string msg = "PRIVMSG " + _requestor + " :" + "Try again: " + clue + left + " tries left, enter [Tips] for hints\r\n";
			send(_clientSocket, msg.c_str(), msg.length(), 0);
		}
	}
}

void	Bot::tips()
{
	if (_t == 0)
	{
		std::string hint;
		if (_num_bot >= 0 && _num_bot <= 500)
			hint = "The number is between 0 and 500";
		else if (_num_bot >= 501 && _num_bot <= 1000)
			hint = "The number is between 501 and 1000";
		std::string msg = "PRIVMSG " + _requestor + " :" + "Tips: " + hint + "\r\n";
		send(_clientSocket, msg.c_str(), msg.length(), 0);
		
	}
	else if (_t == 1)
	{
		std::string hint;
		if (_num_bot % 2 == 0)
			hint = "The number is even";
		else if (_num_bot % 2 != 0)
			hint = "The number is odd";
		std::string msg = "PRIVMSG " + _requestor + " :" + "Tips: " + hint + "\r\n";
		send(_clientSocket, msg.c_str(), msg.length(), 0);
	}
	else if (_t == 2)
	{
		auto hint = std::to_string(_num_bot);
		int size = hint.size();
		auto h = std::to_string(size);
		std::string msg = "PRIVMSG " + _requestor + " :" + "Tips: the number have " + h + " digits\r\n";
		send(_clientSocket, msg.c_str(), msg.length(), 0);
		
	}
	else if (_t == 3)
	{
		auto h = std::to_string(_num_bot);
		if (h.size() > 0)
		{
			char hint = h[0];
			std::string msg = "PRIVMSG " + _requestor + " :" + "Tips: the first digit of the number is " + hint + "\r\n";
			send(_clientSocket, msg.c_str(), msg.length(), 0);
		}
	}
	else if (_t > 3)
	{
		std::string msg = "PRIVMSG " + _requestor + " :" + "You have already 4 tips\r\n";
		send(_clientSocket, msg.c_str(), msg.length(), 0);
	}
}

void	Bot::get_random_num()
{
	long long max = 1000;
	long long min = 0;
	std::srand(std::time(0));
	_num_bot = min + static_cast<long long>(std::rand()) % (max - min + 1);
	std::string msg = "PRIVMSG " + _requestor + " :" + "bot is choosing his number between 0 and 1000\r\n";
	send(_clientSocket, msg.c_str(), msg.length(), 0);
	std::string use = "PRIVMSG " + _requestor + " :" + "bot has chosen: you can use [Stop] or [Number] or [Tips]\r\n";
	send(_clientSocket, use.c_str(), use.length(), 0);
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

long long	ft_atoi(std::string nptr)
{
	long	n;
	int		i;
	int		signe;

	i = 0;
	signe = 1;
	n = 0;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == 32)
		i++;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		n = (n * 10) + nptr[i] - '0';
		i++;
	}
	return (n * signe);
}

//===========================================CHIFOUMI===========================================================//

void	Bot::chifoumi(std::vector<std::string> arg)
{
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


//===========================================UTILS===========================================================//

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
		throw except("Usage: ./a.out <port> <password>");
}

int	Bot::get_fd() const
{
	return _clientSocket;
}

void signal_send(int signum) {
	(void) signum;
	signal_value_bot = true;
	send(client_socket_global, "QUIT :leaving\r\n", 15 , 0);
	close(client_socket_global);
}
