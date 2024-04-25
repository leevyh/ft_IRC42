/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lazanett <lazanett@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 15:22:43 by lazanett          #+#    #+#             */
/*   Updated: 2024/04/25 17:52:53 by lazanett         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"
#include "Server.hpp"

// void	Server::init_bot()
int main(void)
{
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0); // creating socket

	sockaddr_in serverAddress; // specifying address
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(6667);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	connect(clientSocket, (struct sockaddr*)&serverAddress, // sending connection request
			sizeof(serverAddress));

	// sending data 
	// const char* message = "Hello, server!";
	// send(clientSocket, message, strlen(message), 0);
	std::cout << "BOT listening on port " << serverAddress.sin_port << std::endl;
	std::cout << "BOT port: " << ntohs(serverAddress.sin_port) << std::endl;

	close(clientSocket);// closing socket
}