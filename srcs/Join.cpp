#include "Commands.hpp"

short is_Authorize(Server &server, User &user, Channel &channel, std::string key) {
	(void)server;
	(void)user;
	if (!channel.get_ChannelKey().empty() && channel.is_ValidKey(key) == false)
		return (2);
	if (channel.get_limitUser() > -1 && channel.get_ChannelUser().size() >= (unsigned long)channel.get_limitUser())
		return (3);
	if (channel.is_inviteOnly() && !channel.is_InInviteList(user))
		return (4);
	return (1);
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

/* Command: JOIN | Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"

Numeric Replies: ERR_INVITEONLYCHAN (473); ERR_BADCHANNELKEY (475);
				ERR_CHANNELISFULL (471); ERR_BADCHANMASK (476);
				ERR_NOSUCHCHANNEL (403); RPL_TOPIC (332) */
void Commands::join(Server &server, User &user, std::vector<std::string> &arg) {
	std::vector<std::string> channels;
	channels = split(arg[1]);
	std::string msg_send;
	// std::vector<std::string> key;
	short authorized = 0;
	if (check_channelName(server, user, channels) == -6969)
		return;
	for(size_t i = 0; i < channels.size(); i++) {
		if (!(server.get_channels().empty())) {
			authorized = 0;
			for (std::vector<Channel>::iterator it = server.get_channels().begin(); \
				it != server.get_channels().end(); ++it) {
				if (channels[i] == it->get_ChannelName()) {
					// authorized = is_Authorize(server, user, *it, key, i);
					std::string key = arg[arg.size() - 1];
					authorized = is_Authorize(server, user, *it, key);
					switch (authorized) {
						case 1:																	// OK
							it->set_ChannelUser(user);
							displayInfosChannel(server, user, *it);
							if (!it->get_ChannelTopic().empty())
								server.sendMsg(user, RPL_TOPIC(user, *it), 1);
							break;
						case 2:																	// ERR_BADCHANNELKEY
							server.sendMsg(user, ERR_BADCHANNELKEY(user, *it), 1);
							break;
						case 3:																	// ERR_CHANNELISFULL
							server.sendMsg(user, ERR_CHANNELISFULL(user, *it), 1);
							break;
						case 4:																	// ERR_INVITEONLYCHAN
							server.sendMsg(user, ERR_INVITEONLYCHAN(user, *it), 1);
							break;
						default:
							std::cerr << "Error: Unknown error" << std::endl;
					}
				}
			}
		}
		if (authorized == 0)
			create_NewChannel(server, user, channels[i]);
	}
	return;
}
