#pragma once
#include <iostream>
#include <string>
#include <Server.hpp>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <vector>
#include <poll.h>
#include <sys/types.h>

extern bool signal_value;

void	check_args(int argc, char **argv);
void	signal_send(int signum);
