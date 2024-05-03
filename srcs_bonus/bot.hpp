/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lazanett <lazanett@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/30 11:17:56 by lazanett          #+#    #+#             */
/*   Updated: 2024/05/03 18:44:13 by lazanett         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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

class User;
class Commands;
class Channel;
class Server;
class Bot {

	public:
	
		Bot();
		Bot(char **av);
		~Bot();
// GETTERS
		void set_nickname(char * &nickname);

// FUNCTIONS
		void init_bot(bool signal_value);
		void sendMsg(Bot &bot, std::string message, int code) const;
		
		void signalHandler(int signal);
		//COMMAND BOT
		void bot_command(std::vector<std::string> command);
		void man(Server &server, User &user);
		void connexion(Server &server, User &user, Channel chan, std::string channelname);
		void list(Server &server, User &user);

	private:

		struct sockaddr_in	_server_addr;
		bool				_signal_value;
		const char			*_serverIP;
		long				_port;
		int					_clientSocket;
		const char			*_nickname;
		const char			*_username;
		std::string			_pass;
		std::string			_requestor;
};

void check_args_bonus(int argc, char **argv);
std::vector<std::string>	split(const std::string& str, char del);
std::vector<std::string>	split_space(const std::string& str);
std::vector<std::string>	split_pre_requestor(const std::string& str);
std::vector<std::string>	split_requestor(const std::string& str);