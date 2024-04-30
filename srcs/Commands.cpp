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
	cmdMap["KICK"] = &Commands::kick;
	cmdMap["INVITE"] = &Commands::invite;
	cmdMap["WHOIS"] = &Commands::whois;
	cmdMap["WHO"] = &Commands::who;
}

Commands::~Commands(void) {}

/* ************************************************************************** */

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

/* ************************************************************************** */

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
	if (arg[1].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]\\\\`_^{|}-") \
		!= std::string::npos)
		return (server.sendMsg(user, ERR_ERRONEUSNICKNAME(arg[1]), 1));
	std::map<int, User> users = server.get_clientmap();
	int code = 0;
	for(std::map<int, User>::iterator it = server.get_clientmap().begin(); \
		it != server.get_clientmap().end(); ++it) {
		if (arg[1] == it->second.get_nickname())
		{
			server.sendMsg(user, ERR_NICKNAMEINUSE(arg[1]), 1);
			code = 1;
		}
	}
	if (!user.get_nickname().empty() && code == 0) {
		server.sendMsg(user, user.get_nickname() + " changed his nickname to " + arg[1], 1);
		std::string resp = user.get_nickname() + "!~" + user.get_username() + "@" + user.get_ip() \
		+ " NICK :" + arg[1];
		server.sendMsg(user, resp, 2);
	}
	if (code == 0)
	user.set_nickname(arg[1]);
}


/* Command USER | Parameters: <user> <mode> <unused> <realname> */
void Commands::user(Server &server, User &user, std::vector<std::string> &arg) {
	if (user.get_authenticated() == true)
		return (server.sendMsg(user, ERR_ALREADYREGISTRED(user), 1));
	if (arg.size() < 5)
		return (server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "USER"), 1));
	user.set_username(arg[1]);
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

/* ************************************************************************** */

/* Command: JOIN | Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0" */
void Commands::join(Server &server, User &user, std::vector<std::string> &arg) {
	std::vector<std::string> channels;
	channels = split(arg[1]);
	std::string msg_send;
	short authorized = 0;
	if (check_channelName(server, user, channels) == -6969)
		return;
	for(size_t i = 0; i < channels.size(); i++) {
		if (!(server.get_channels().empty())) {
			authorized = 0;
			for (std::vector<Channel>::iterator it = server.get_channels().begin(); \
				it != server.get_channels().end(); ++it) {
				if (channels[i] == it->get_ChannelName()) {
					std::string key = arg[arg.size() - 1];
					authorized = is_Authorize(user, *it, key);
					switch (authorized) {
						case 1:
							it->set_ChannelUser(user);
							displayInfosChannel(server, user, *it);
							if (!it->get_ChannelTopic().empty())
								server.sendMsg(user, RPL_TOPIC(user, *it), 1);
							break;
						case 2:
							server.sendMsg(user, ERR_BADCHANNELKEY(user, *it), 1);
							break;
						case 3:
							server.sendMsg(user, ERR_CHANNELISFULL(user, *it), 1);
							break;
						case 4:
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

/* Command: INVITE | Parameters: <nickname> <channel> */
void Commands::invite(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg.size() != 3)
		return (server.sendMsg(user, ERR_NEEDMOREPARAMS(user, arg[0]), 1));
	if (!arg[1].empty() && server.is_onServer(arg[1]) == false)
		return (server.sendMsg(user, ERR_NOSUCHNICK(user, arg[1]), 1));
	for (std::vector<Channel>::iterator itc = server.get_channels().begin(); 
		itc != server.get_channels().end(); ++itc) {
		if (itc->get_ChannelName() == arg[2]) {
			if (itc->is_UserInChannel(user)) {
				if (itc->is_opChannel(user.get_nickname()) == false)
					return (server.sendMsg(user, ERR_CHANOPRIVSNEEDED(user, *itc), 1));
				else {
					for (std::vector<User>::iterator itu = itc->get_ChannelUser().begin(); \
						itu != itc->get_ChannelUser().end(); ++itu)
						if (itu->get_nickname() == arg[1])
							return (server.sendMsg(user, ERR_USERONCHANNEL(user, arg[1], *itc), 1));
				}
			}
			else
				return (server.sendMsg(user, ERR_NOTONCHANNEL(user, *itc), 1));
		}
		return (displayInvite(server, user, *itc, arg[1]));
	}
	return (server.sendMsg(user, ERR_NOSUCHCHANNEL(user, arg[2]), 1));
}

/* Command: KICK | Parameters: <channel> *( "," <channel> ) <user> *( "," <user> ) [<comment>] */
void Commands::kick(Server &server, User &user, std::vector<std::string> &arg) {
	std::string kick_msg;
	if (arg.size() < 3)
		return (server.sendMsg(user, ERR_NEEDMOREPARAMS(user, arg[0]), 1));
	if (!arg[2].empty() && server.is_onServer(arg[2]) == false)
		return (server.sendMsg(user, ERR_NOSUCHNICK(user, arg[1]), 1));
	kick_msg = remove_OneChar(':', arg, 3);
	for (std::vector< Channel>::iterator itc = server.get_channels().begin(); 
		itc != server.get_channels().end(); ++itc) {
		if (itc->get_ChannelName() == arg[1]) {
			if (itc->is_UserInChannel(user)) {
				if (itc->is_opChannel(user.get_nickname()) == false)
					return (server.sendMsg(user, ERR_CHANOPRIVSNEEDED(user, *itc), 1));
				else {
					for (std::vector<User>::iterator itu = itc->get_ChannelUser().begin(); \
						itu != itc->get_ChannelUser().end(); ++itu) {
						std::cout << "Present: " << itu->get_nickname() << std::endl;
						if (itu->get_nickname() == arg[2]) {
							itc->sendMsg(user, RPL_KICK(user, *itc, arg[2], kick_msg), 1);
							itc->remove_inviteList(*itu);
							itc->unset_ChannelUser(*itu);
							if (itc->get_ChannelUser().empty())
								server.remove_channelList(*itc);
							return;
						}
					}
					return (server.sendMsg(user, ERR_USERNOTINCHANNEL(user, arg[2], *itc), 1));
				}
			}
			return (server.sendMsg(user, ERR_NOTONCHANNEL(user, *itc), 1));
		}
	}
	return (server.sendMsg(user, ERR_NOSUCHCHANNEL(user, arg[1]), 1));
}

/* Command: PART | Parameters: <channel> *( "," <channel> ) [ <Part Message> ] */
void Commands::part(Server &server, User &user, std::vector<std::string> &arg) {
	std::vector<std::string> channels;
	channels = split(arg[1]);
	std::string part_msg;
	int code = 0;
	int j = 0;
	if (arg.size() >= 3) {
		part_msg = remove_OneChar(':', arg, 2);
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
			for (std::vector<Channel>::iterator it = server.get_channels().begin();
				 it != server.get_channels().end(); ++it) {
				j = 0;
				if (channels[i] == it->get_ChannelName()) {
					j = 1;
					if (it->is_UserInChannel(user)) {
						it->unset_ChannelUser(user);
						server.sendMsg(user, RPL_PART(user, *it, part_msg, code), 2);
						if (it->get_ChannelUser().empty()) {
							server.remove_channelList(*it);
							break;
						}
						else {
							it->sendMsg(user, RPL_PART(user, *it, part_msg, code), 2);
							break;
						}
					}
					else {
						server.sendMsg(user, ERR_NOTONCHANNEL(user, *it), 1);
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

/* Command: TOPIC | Parameters: <channel> [ <topic> ] */
void Commands::topic(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg.size() >= 3) {
		if (!server.get_channels().empty()) {
			for (std::vector<Channel>::iterator it = server.get_channels().begin(); \
				it != server.get_channels().end(); ++it) {
				if (arg[1] == it->get_ChannelName())
					edit_Topic(server, user, arg, *it);
				else
					continue;
				return ;
			}
			server.sendMsg(user, ERR_NOSUCHCHANNEL(user, arg[1]), 1);
		}
	}
	if (arg.size() == 2) {
		if (!server.get_channels().empty()) {
			for(std::vector< Channel>::iterator it = server.get_channels().begin(); \
				it != server.get_channels().end(); ++it) {
				if (arg[1] != it->get_ChannelName())
					server.sendMsg(user, ERR_NOTONCHANNEL(user, *it), 1);
			}
		}
	}
	else
		server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "TOPIC"), 1);
}

/* Command: QUIT | Parameters: [ <Quit Message> ] */
void Commands::quit(Server &server, User &user, std::vector<std::string> &arg) {
	int nb_arg = 0;
	std::string msg_send = "Quit: ";
	for (std::vector<std::string>::iterator it = arg.begin(); it != arg.end(); ++it)
		nb_arg++;
	if (nb_arg > 2 && arg[1][0] != ':')
		server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "QUIT"), 1);
	if (nb_arg > 1) {
		if (arg[1][0] == ':') {
			msg_send += arg[1].erase(0, 1) + " ";
			for (size_t i = 2; i < arg.size(); i++)
				msg_send += arg[i] + " ";
		}
		else
			msg_send += arg[1];
	}
	std::vector<std::vector<Channel>::iterator> channel_list;
	if (!server.get_channels().empty()) {
		for (std::vector<Channel>::iterator it = server.get_channels().begin(); \
			it != server.get_channels().end(); ++it) {
			if (it->is_UserInChannel(user)) {
				it->unset_ChannelUser(user);
				if (it->get_ChannelUser().empty())
					channel_list.push_back(it);
				else
					it->sendMsg(user, RPL_QUIT(user, msg_send), 1);
			}
		}
	}
	for (std::vector<std::vector<Channel>::iterator>::iterator it = channel_list.begin(); \
		it != channel_list.end(); ++it)
		server.get_channels().erase((*it));
	user.set_authenticated(false);
	user.set_status(false);
}

/* ************************************************************************** */

/* Command: PRIVMSG | Parameters: <target>{,<target>} <text to be sent> */
void Commands::privmsg(Server &server, User &user, std::vector<std::string> &arg) {
	std::string full_msg;
	if (arg.size() < 3) {
		return (server.sendMsg(user, ERR_NORECIPIENT(user, "PRIVMSG"), 1));
	}
	full_msg = remove_OneChar(':', arg, 2);
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
		for (std::vector<Channel>::iterator it = server.get_channels().begin();
			 it != server.get_channels().end(); it++)
			if (arg[1] == it->get_ChannelName()) {
				std::vector<User> user_list = it->get_ChannelUser();
				for (std::vector<User>::iterator ita = user_list.begin(); ita != user_list.end(); ++ita)
					if (ita->get_fd() == user.get_fd()) {
						it->sendMsg(user, RPL_PRIVMSG(user, arg[1], full_msg), 2); // A VERIFIER
						return;
					}
			}
		server.sendMsg(user, ERR_CANNOTSENDTOCHAN(user, arg[1]), 1);
	}
}


/* ************************************************************************** */

/* Command: PING | Parameters: <token> */
void Commands::ping(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg.size() != 2 || arg[1].empty() || !arg[1].size())
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

/* ************************************************************************** */

/* Command: WHOIS | Parameters: [<target>] <nick> */
void Commands::whois(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg.size() != 2)
		return (server.sendMsg(user, ERR_NEEDMOREPARAMS(user, arg[0]), 1));
	if (!arg[1].empty() && server.is_onServer(arg[1]) == false)
		return (server.sendMsg(user, ERR_NOSUCHNICK(user, arg[1]), 1));
	for (std::map<int, User>::iterator it = server.get_clientmap().begin(); \
		it != server.get_clientmap().end(); ++it) {
		if (it->second.get_nickname() == arg[1]) {
			server.sendMsg(user, RPL_WHOISUSER(user, it->second), 1);
			server.sendMsg(user, RPL_WHOISSERVER(user, it->second, server), 1);
			server.sendMsg(user, RPL_ENDOFWHOIS(user, it->second), 1);
			return;
		}
	}
	return (server.sendMsg(user, ERR_NOSUCHNICK(user, arg[1]), 1));
}

/*Command: WHO | Parameters: <mask> */
void Commands::who(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg.size() != 2)
		return ;
	if (!arg[1].empty() && server.is_onServer(user.get_nickname()) == true) {
		for (std::vector<Channel>::iterator it = server.get_channels().begin(); \
			it != server.get_channels().end(); ++it) {
				if (it->get_ChannelName() == arg[1]) {
					server.sendMsg(user, RPL_WHOREPLY(server, user, *it), 1);
					server.sendMsg(user, RPL_ENDOFWHO(user, *it), 1);
				}
			}
	}
}
