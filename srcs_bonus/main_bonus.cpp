/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lazanett <lazanett@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 16:48:14 by lazanett          #+#    #+#             */
/*   Updated: 2024/04/25 17:46:45 by lazanett         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "IRC.hpp"
#include "User.hpp"
#include "unistd.h"
#include <algorithm>

bool signal_value = false;

int main(int argc, char **argv) {
	//try {
	check_args(argc, argv);
	std::signal(SIGINT, &signal_send);
	Server serv(strtol(argv[1], NULL, 10), argv[2]);
	serv.init_serv();
	while (!signal_value)
		serv.start_serv();
	// }
}