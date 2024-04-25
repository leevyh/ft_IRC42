#include "Commands.hpp"

Commands::Commands(void) {
	cmdMap["CAP"] = &Commands::capls;
	cmdMap["PASS"] = &Commands::pass;
	cmdMap["NICK"] = &Commands::nick;
	cmdMap["USER"] = &Commands::user;
	cmdMap["QUIT"] = &Commands::quit;
	cmdMap["MODE"] = &Commands::mode;
	cmdMap["PING"] = &Commands::ping;
	cmdMap["PONG"] = &Commands::pong;
	cmdMap["PRIVMSG"] = &Commands::privmsg;
	cmdMap["JOIN"] = &Commands::join;
	cmdMap["TOPIC"] = &Commands::topic;
	cmdMap["PART"] = &Commands::part;
	// cmdMap["KICK"] = &Commands::kick;
	cmdMap["INVITE"] = &Commands::invite;
}

Commands::~Commands(void) {}

void Commands::getcommand(Server &server, User &user, std::vector<std::string> &argument) {
	bool command = false;

	if (!argument.empty()) {
		for (std::map<std::string, cmdFPtr>::iterator it = cmdMap.begin(); it != cmdMap.end(); ++it) { 
			if (it->first == argument[0]) {
				(this->*(it->second))(server, user, argument); 
				command = true;
			}
		}
		if (command == false)
			server.sendMsg(user, ERR_UNKNOWNCOMMAND(user, argument[0]), 1);
	}
	else
		server.sendMsg(user, ERR_UNKNOWNCOMMAND(user, ""), 1);
	return;
}

void Commands::capls(Server &server, User &user, std::vector<std::string> &arg) { 
	if (arg.size() > 1 && arg[1] == "LS")
		server.sendMsg(user, "CAP * LS :none", 1);
}

/* Command PASS | Parameters: <password> */
void Commands::pass(Server &server, User &user, std::vector<std::string> &arg) {
	if (user.get_authenticated() == true)
		return (server.sendMsg(user, ERR_ALREADYREGISTRED(user), 1));
	if (arg[1].empty()) {
		server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "PASS"), 1);
		server.disconnect(user);
		return;
	}
	if (arg[1] == server.get_Password()) {
		if (user.get_password().empty()) {
			user.set_password(arg[1]);
			return;
		}
		return (server.sendMsg(user, ERR_ALREADYREGISTRED(user), 1));
	}
	server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "PASS"), 1);
	server.disconnect(user);
	return;
}

/* Command NICK | Parameters: <nickname> */
void Commands::nick(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg[1].empty())
		return (server.sendMsg(user, ERR_NONICKNAMEGIVEN(arg[1]), 1));
	if (arg[1].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") \
		!= std::string::npos) //ajouter les autres caractères autorisés
		return (server.sendMsg(user, ERR_ERRONEUSNICKNAME(arg[1]), 1));
	std::map<int, User> users = server.get_clientmap();
	for(std::map<int, User>::iterator it = server.get_clientmap().begin(); \
		it != server.get_clientmap().end(); ++it) {
		if (arg[1] == it->second.get_nickname())
			server.sendMsg(user, ERR_NICKNAMEINUSE(arg[1]), 1);
	}
	if (!user.get_nickname().empty()) {
		server.sendMsg(user, user.get_nickname() + " changed his nickname to " + arg[1], 1);
		std::string resp = user.get_nickname() + "!~" + user.get_username() + "@" + user.get_ip() \
		+ " NICK :" + arg[1];
		server.sendMsg(user, resp, 2);
	}
	user.set_nickname(arg[1]);
}

/* Command USER | Parameters: <user> <mode> <unused> <realname> */
void Commands::user(Server &server, User &user, std::vector<std::string> &arg) {
	if (user.get_authenticated() == true)
		return (server.sendMsg(user, ERR_ALREADYREGISTRED(user), 1));
	if (arg.size() < 5)
		return (server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "USER"), 1));
	user.set_username(arg[1]);
	user.set_ip(arg[3]);
	if (arg[4][0] == ':') {
		std::string realname;
		arg[4] = arg[4].erase(0, 1);
		for (size_t i = 4; i < arg.size(); i++) {
			realname += arg[i];
			realname += " ";
		}
		user.set_realname(realname);
	}
	else
		user.set_realname(arg[4]);
}


/* Command: QUIT | Parameters: [ <Quit Message> ]
Command Example:   QUIT :Gone to have lunch         ; Client exiting from the network
Message Example:  :dan-!d@localhost QUIT :Quit: Bye for now!
					; dan- is exiting the network with the message: "Quit: Bye for now!" */
void Commands::quit(Server &server, User &user, std::vector<std::string> &arg) {
	int nb_arg = 0;
	std::string msg_send = "Quit: ";
	for (std::vector<std::string>::iterator it = arg.begin(); it != arg.end(); ++it) {
		nb_arg++;
	}
	if (nb_arg > 2 && arg[1][0] != ':')
		server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "QUIT"), 1);
	if (nb_arg > 1)
	{
		if (arg[1][0] == ':')
		{
			msg_send += arg[1].erase(0, 1) + " ";
			for (size_t i = 2; i < arg.size(); i++)
				msg_send += arg[i] + " ";
		}
		else
			msg_send += arg[1];
	}
	std::vector<std::map<std::string, Channel>::iterator> channel_list;
	if (!server.get_channels().empty())
	{
		for (std::map<std::string, Channel>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); ++it)
		{
			std::cout << "user.get_nickname(): " << user.get_nickname() << std::endl;
			if (it->second.is_UserInChannel(user))
			{
				std::cout << "user removed from channel: " << user.get_nickname() << std::endl;
				it->second.unset_ChannelUser(user);
				if (it->second.get_UserChannel().empty())
				{
					channel_list.push_back(it);
				}
				else
				{
					std::vector<User> user_list = it->second.get_UserChannel();
					//DISPLAY LIST OF USER IN CHANNEL
					for (std::vector<User>::iterator ita = user_list.begin(); ita != user_list.end(); ++ita)
					{
						std::cout << "ita->get_fd(): " << ita->get_fd() << std::endl;
						std::cout << "ita->get_nickname(): " << ita->get_nickname() << std::endl;
					}
					std::string final_message = user.get_nickname() + "!~" + user.get_username() + "@" + user.get_ip() + ".ip QUIT :" + msg_send;
					it->second.sendMsg(user, final_message, 1);
				}
			}
		}
	}
	for (std::vector<std::map<std::string, Channel>::iterator>::iterator it = channel_list.begin(); it != channel_list.end(); ++it)
		server.get_channels().erase((*it)->first);
	//Display all channels always active
	for (std::map<std::string, Channel>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); ++it)
	{
		std::cout << it->second.get_ChannelName() << std::endl;
	}
	user.set_authenticated(false);
	user.set_status(false);
	std::cout << "QUIT a coder" << std::endl;
}



/* --------------------------------------------------JOIN-------------------------------------------------- */

/* Command: JOIN | Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"

Numeric Replies: ERR_INVITEONLYCHAN (473); ERR_BADCHANNELKEY (475);
				ERR_CHANNELISFULL (471); ERR_BADCHANMASK (476);
				ERR_NOSUCHCHANNEL (403); RPL_TOPIC (332) */

void	channel_BroadcastJoin(Server &server, User &user, std::string const &channel_name)
{
	std::string msg_send;
	std::vector<User> user_list = server.get_channels()[channel_name].get_UserChannel();
	for (std::vector<User>::iterator it = user_list.begin(); it != user_list.end(); ++it)
	{
//		if (it->get_fd() != user.get_fd())
//		{
		msg_send = user.get_nickname() + "!~" + user.get_username() + "@localhost.ip JOIN :" + channel_name;
		server.sendMsg(*it, msg_send, 2);
//		}
	}
}


short is_Authorize(Server &server, User &user, Channel &channel, std::vector<std::string> &key, size_t i) {
	(void)server;
	(void)user;
	if (!channel.get_password().empty() && channel.is_ValidKey(key, i) == false)
		return (2);
	if (channel.get_limitUser() > -1 && channel.get_UserChannel().size() >= (unsigned long)channel.get_limitUser())
		return (3);
	if (channel.is_inviteOnly())
		return (4);
	return (1);
}


void Commands::join(Server &server, User &user, std::vector<std::string> &arg) {
	std::vector<std::string> channels;
	channels = split(arg[1]);
	std::string msg_send;
	std::vector<std::string> key;
	short authorized = 0;
	if (check_channelName(server, user, channels) == -6969)
		return;
	for(size_t i = 0; i < channels.size(); i++) {
		if (!(server.get_channels().empty())) {
			authorized = 0;
			for (std::map<std::string, Channel>::iterator it = server.get_channels().begin(); \
				it != server.get_channels().end(); ++it) {
				if (channels[i] == it->second.get_ChannelName()) {
					authorized = is_Authorize(server, user, it->second, key, i);
					switch (authorized) {
						case 1:
							add_UserInChannel(server, user, it->second);   // A VERIFIER
							if (!it->second.get_ChannelTopic().empty())
								server.sendMsg(user, RPL_TOPIC(user, it->second), 1);
							break;
						case 2:
							server.sendMsg(user, ERR_BADCHANNELKEY(user, it->second), 1);
							break;
						case 3: //channel full
							std::cout << "ERR_CHANNELISFULL a coder" << std::endl;
//							server.sendMsg(user, ERR_CHANNELISFULL(user, it->second), 1);
							break;
						case 4:  //channel sur invite
							std::cout << "ERR_INVITEONLYCHAN a coder" << std::endl;
							// server.sendMsg(user, ERR_INVITEONLYCHAN(user, it->second), 1);
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
	server.get_channels()[channel_name] = channel;
	channel_BroadcastJoin(server, user, channel.get_ChannelName());
	displayInfosChannel(server, user, channel);
}

void	add_UserInChannel(Server &server, User &user, Channel &channel) {
	channel.set_ChannelUser(user);
	channel_BroadcastJoin(server, user, channel.get_ChannelName());
	displayInfosChannel(server, user, channel);
}


/* --------------------------------------------------TOPIC-------------------------------------------------- */


void	edit_Topic(Server &server, User &user, std::vector<std::string> &arg, Channel &chan)
{
	if (arg[1] == chan.get_ChannelName()) {
		if (chan.is_opChannel(user.get_nickname())) {
			std::string topic = remove_OneChar(':', arg);
			chan.set_ChannelTopic(topic);
			for (std::vector<User>::iterator it = chan.get_UserChannel().begin(); \
				it != chan.get_UserChannel().end(); ++it)
				server.sendMsg(*it, RPL_EDITTOPIC(user, chan, topic), 2);
		}
		else
			server.sendMsg(user, ERR_CHANOPRIVSNEEDED(user, chan), 1);
	}
}

void Commands::topic(Server &server, User &user, std::vector<std::string> &arg) {
	(void)user;
	if (arg.size() >= 3) {
		if (!server.get_channels().empty()) {
			for (std::map<std::string, Channel>::iterator it = server.get_channels().begin(); \
				it != server.get_channels().end(); ++it) {
				if (arg[1] == it->second.get_ChannelName())
					edit_Topic(server, user, arg, it->second);
				else
					continue;
				return ;
			}
			server.sendMsg(user, ERR_NOSUCHCHANNEL(user, arg[1]), 1);
		}
	}
	if (arg.size() == 2) {
		if (!server.get_channels().empty()) {
			for(std::map<std::string, Channel>::iterator it = server.get_channels().begin(); \
				it != server.get_channels().end(); ++it) {
				if (arg[1] != it->second.get_ChannelName())
					server.sendMsg(user, ERR_NOTONCHANNEL(user, it->second), 1);
			}
		}
	}
	else
		server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "TOPIC"), 1);
}


/* Command: PING | Parameters: <token> */
void Commands::ping(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg[1].empty() || !arg[1].size())
		return (server.sendMsg(user, ERR_NOORIGIN(user), 1));
	std::string msg_send = "PONG :" + arg[1];
	server.sendMsg(user, msg_send, 1);
}


/* Command: PONG | Parameters: [<server>] <token> */
void Commands::pong(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg[1].empty() || !arg[1].size())
		return (server.sendMsg(user, ERR_NOORIGIN(user), 1));
	user.set_lastping(time(NULL));
}


/* Command: PRIVMSG | Parameters: <target>{,<target>} <text to be sent> */
void Commands::privmsg(Server &server, User &user, std::vector<std::string> &arg) {
	std::string full_msg;
	if (arg.size() < 3) {
		return (server.sendMsg(user, ERR_NORECIPIENT(user, "PRIVMSG"), 1));
	}
	full_msg = remove_OneChar(':', arg);
	if (arg[1][0] != '#') {
		for (std::map<int, User>::iterator it = server.get_clientmap().begin();
			 it != server.get_clientmap().end(); ++it)
			if (arg[1] == it->second.get_nickname()) {
				server.sendMsg(it->second, RPL_PRIVMSG(user, arg[1], full_msg), 2); // A VERIFIER
				return;
			}
		server.sendMsg(user, ERR_NOSUCHNICK(user, arg[1]), 1);
	}
	else {
		for (std::map<std::string, Channel>::iterator it = server.get_channels().begin();
			 it != server.get_channels().end(); it++)
			if (arg[1] == it->second.get_ChannelName()) {
				std::vector<User> user_list = it->second.get_UserChannel();
				for (std::vector<User>::iterator ita = user_list.begin(); ita != user_list.end(); ++ita)
					if (ita->get_fd() == user.get_fd()) {
						it->second.sendMsg(user, RPL_PRIVMSG(user, arg[1], full_msg), 2); // A VERIFIER
						return;
					}
			}
		server.sendMsg(user, ERR_CANNOTSENDTOCHAN(user, arg[1]), 1);
	}
}

void Commands::part(Server &server, User &user, std::vector<std::string> &arg) {
	std::vector<std::string> channels;
	channels = split(arg[1]);
	std::string part_msg;
	int code = 0;
	int j = 0;
	if (arg.size() >= 3) {
		part_msg = remove_OneChar(':', arg);
		std::cout << "part_msg: " << part_msg << std::endl;
		code = 2;
	}
	else if (arg.size() == 2) {
		part_msg = "";
		code = 1;
	}
	else {
		server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "PART"), 1);
		return;
	}
	if (check_channelName(server, user, channels) == -6969)
		return;
	for (size_t i = 0; i < channels.size(); i++) {
		std::vector<std::map<std::string, Channel>::iterator> channel_list;
		if (!server.get_channels().empty()) {
			for (std::map<std::string, Channel>::iterator it = server.get_channels().begin();
				 it != server.get_channels().end(); ++it) {
				j = 0;
				if (channels[i] == it->second.get_ChannelName()) {
					j = 1;
					if (it->second.is_UserInChannel(user)) {
						it->second.unset_ChannelUser(user);
						server.sendMsg(user, RPL_PART(user, it->second, part_msg, code), 2);
						if (it->second.get_UserChannel().empty()) {
							channel_list.push_back(it);
							for (std::vector<std::map<std::string, Channel>::iterator>::iterator ita = channel_list.begin();
								 ita != channel_list.end(); ++ita)
								server.get_channels().erase((*ita)->first);
							break;
						}
						else {
							it->second.sendMsg(user, RPL_PART(user, it->second, part_msg, code), 2);
							break;
						}
					}
					else {
						server.sendMsg(user, ERR_NOTONCHANNEL(user, it->second), 1);
						break;
					}
				}
				else
					continue;
			}
			if (j == 0)
				server.sendMsg(user, ERR_NOSUCHCHANNEL(user, channels[i]), 1);
		}
		else
			server.sendMsg(user, ERR_NOSUCHCHANNEL(user, arg[i]), 1);
	}
}

/* Command: INVITE | Parameters: <nickname> <channel> */
void Commands::invite(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg.size() != 3)
		return (server.sendMsg(user, ERR_NEEDMOREPARAMS(user, arg[0]), 1));
	if (!arg[1].empty() && server.is_onServer(arg[1]) == false)
		return (server.sendMsg(user, ERR_NOSUCHNICK(user, arg[1]), 1));
	for (std::map<std::string, Channel>::iterator itc = server.get_channels().begin(); 
		itc != server.get_channels().end(); ++itc) {
		if (itc->first == arg[2]) {
			for (std::vector<User>::iterator itu = itc->second.get_UserChannel().begin(); \
				itu != itc->second.get_UserChannel().end(); ++itu) {
				if (itu->get_nickname() == user.get_nickname()) {
					if (itc->second.is_opChannel(user.get_nickname()) == false)
						return (server.sendMsg(user, ERR_CHANOPRIVSNEEDED(user, itc->second), 1));
					else
						if (itu->get_nickname() == arg[1])
							return (server.sendMsg(user, ERR_USERONCHANNEL(user, arg[1], itc->second), 1));
				}
				else
					return (server.sendMsg(user, ERR_NOTONCHANNEL(user, itc->second), 1));
			}
			return (displayInvite(server, user, itc->second, arg[1]));
		}
	}
	return (server.sendMsg(user, ERR_NOSUCHCHANNEL(user, arg[2]), 1));
}
