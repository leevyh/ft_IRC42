#include "IRC.hpp"

void check_args(int argc, char **argv) {
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

void signal_send(int signum) {
	(void) signum;
	signal_value = true;
}

/* ************************************************************************** */

std::string remove_OneChar(char c, std::vector<std::string> &arg, int i) {
	std::string full_msg;
	for (std::vector<std::string>::iterator it = arg.begin() + i; it != arg.end(); ++it, i++) {
		if (arg[i][0] == c)
			full_msg += arg[i].erase(0, 1);
		else
			full_msg += " " + *it;
	}
	return (full_msg);
}

std::vector<std::string>	split(const std::string& str) {
	std::vector<std::string> channels_result;
	std::string delimiter = ",";
	char *args = strtok((char *)str.c_str(), ",");
	while (args != NULL) {
		channels_result.push_back(args);
		args = strtok(NULL, ",");
	}
	return (channels_result);
}

/* ************************************************************************** */

short is_Authorize(User &user, Channel &channel, std::string key) {
	if (!channel.get_ChannelKey().empty() && channel.is_ValidKey(key) == false)
		return (2);
	if (channel.get_limitUser() > -1 && channel.get_ChannelUser().size() >= (unsigned long)channel.get_limitUser())
		return (3);
	if (channel.is_inviteOnly() && !channel.is_InInviteList(user))
		return (4);
	return (1);
}

int	check_channelName(Server &server, User &user, std::vector<std::string> &channel) {
	for (size_t i = 0; i < channel.size(); i++) {
		if (channel[i][0] != '#') {
			server.sendMsg(user, ERR_BADCHANMASK(channel[i]), 1);
			return (-6969);
		}
		if (channel[i].find_first_not_of("#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") 
			!= std::string::npos) {
			server.sendMsg(user, ERR_BADCHANMASK(channel[i]), 1);
			return (-6969);
		}
	}
	return (0);
}

void	create_NewChannel(Server &server, User &user, std::string const &channel_name) {
	Channel channel(channel_name);
	channel.set_ChannelUser(user);
	channel.set_opChannel(user.get_nickname());
	channel.creationTime();
	server.add_channelList(channel);
	displayInfosChannel(server, user, channel);
}

/* ************************************************************************** */

void	edit_Topic(Server &server, User &user, std::vector<std::string> &arg, Channel &chan) {
	if (arg[1] == chan.get_ChannelName()) {
		if (chan.is_opChannel(user.get_nickname()) || chan.is_opTopic() == false) {
			std::string topic = remove_OneChar(':', arg, 2);
			chan.set_ChannelTopic(topic);
			for (std::vector<User>::iterator it = chan.get_ChannelUser().begin(); \
				it != chan.get_ChannelUser().end(); ++it)
				server.sendMsg(*it, RPL_EDITTOPIC(user, chan, topic), 2);
		}
		else
			server.sendMsg(user, ERR_CHANOPRIVSNEEDED(user, chan), 1);
	}
}

/* ************************************************************************** */

std::string	print_Names(std::string nickname, Channel &channel) {
	std::vector<std::string> user_list = channel.get_opUsers();
	std::string names = "";
	for (std::vector<std::string>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if (*it == nickname)
			names += "@";
	}
	return (names);
}
