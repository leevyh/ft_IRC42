#include "IRC.hpp"

void check_args(int argc, char **argv) {
	if (argc == 3) {
		for (int i = 0; argv[1][i]; i++) {
			if (!isdigit(argv[1][i])) {
				std::cerr << "Port must be a number" << std::endl;
				//throw exception("Port must be a number")
				exit(1);
			}
		}
		long port = strtol(argv[1], NULL, 10);
		if (port < 1024 || port > 65535) {
			std::cerr << "Port must be between 1024 and 65535" << std::endl;
			//throw exception("Port must be between 1024 and 65535")
			exit(1);
		}
	}
	else {
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		//throw exception("Usage: ./ircserv <port> <password>")
	}
}

std::string remove_OneChar(char c, std::vector<std::string> &arg) {
	std::string full_msg;
	int i = 2;
	for (std::vector<std::string>::iterator it = arg.begin() + 2; it != arg.end(); ++it, i++) {
		if (arg[i][0] == c)
			full_msg += arg[i].erase(0, 1);
		else
			full_msg += " " + *it;
	}
	return (full_msg);
}

void signal_send(int signum) {
	(void) signum;
	signal_value = true;
}

