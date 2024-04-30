/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lazanett <lazanett@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/30 11:17:56 by lazanett          #+#    #+#             */
/*   Updated: 2024/04/30 16:25:48 by lazanett         ###   ########.fr       */
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

class Bot {

	public:
	
		Bot();
		~Bot();
// GETTERS
		void set_nickname(const std::string &nickname);

// FUNCTIONS
		void init_bot();
		void sendMsg(Bot &bot, std::string message, int code) const;
		void bot_command(std::string command);
		void info(void);

	private:

		struct sockaddr_in	_server_addr;
		bool				_signal_value;
		const char			*_serverIP;
		long				_port;
		int					_clientSocket;
		std::string			_nickname;
};
