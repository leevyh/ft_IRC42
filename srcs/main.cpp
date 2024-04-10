#include "Server.hpp"
#include "IRC.hpp"

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
