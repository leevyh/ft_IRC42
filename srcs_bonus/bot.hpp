/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lazanett <lazanett@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/30 11:17:56 by lazanett          #+#    #+#             */
/*   Updated: 2024/05/29 16:15:29 by lazanett         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../includes/IRC.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include <poll.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <csignal>
#include <sstream>

class Bot {

	public:
	
		Bot();
		Bot(char **av);
		~Bot();
// GETTERS
		void set_nickname(char * &nickname);
		int	get_fd() const;
// FUNCTIONS
		void init_bot();
		void sendMsg(Bot &bot, std::string message, int code) const;
		
		void signalHandler(int signal);
		//COMMAND BOT
		void bot_command(std::string command);
		void man();
		void chifoumi(std::vector<std::string> arg);
		void advantage(std::vector<std::string> arg, std::string choice_bot);
		void number(std::vector<std::string> arg);
		int Validcommand(std::vector<std::string> arg);
		int	legit_num(std::vector<std::string> arg);
		void get_random_num();
		void gessnum(std::string command);
		void tips();
		void number();
		int	connexion();



	private:

		struct sockaddr_in	_server_addr;
		bool				_signal_value_bot;
		const char			*_serverIP;
		long				_port;
		int					_clientSocket;
		const char			*_nickname;
		const char			*_username;
		std::string			_pass;
		std::string			_requestor;
		std::string			_msg;
		int					_n_choice_bot;
		int					_n_arg;
		int					_flag_num;
		std::string			_choice_bot;
		std::string			_num;
		long long			_num_bot;
		int					_it;
		int					_it_left;
		long long			_t;
};

void check_args_bonus(int argc, char **argv);
std::vector<std::string>	split(const std::string& str, char del);
std::vector<std::string>	split_space(const std::string& str);
std::vector<std::string>	split_pre_requestor(const std::string& str);
std::vector<std::string>	split_requestor(const std::string& str);
std::string choice();
void shuffle(const char** array, int size);
void sendPrivMsg(int clientSocket, const std::string& recipient, const std::string& message);
long long	ft_atoi(std::string nptr);
void signal_send(int signum);