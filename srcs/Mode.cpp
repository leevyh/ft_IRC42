#include "Commands.hpp"

/*Command: MODE | Parameters: <channel> *( ( "-" / "+" ) *<modes> *<modeparams> ) */
void	set_mode(Server &server, User &user, Channel &chan, std::vector<std::string> &arg) {
	char	mode[] = {'i', 't', 'k', 'o', 'l'};

	for (int i = 0; i < 5; i++) {
		if (mode[i] == arg[2][1]) {
			switch (i) {
				case 0:
					chan.set_inviteOnly();
					for (std::vector<User>::iterator it = chan.get_ChannelUser().begin(); \
						it != chan.get_ChannelUser().end(); ++it)
							chan.add_inviteList(*it);
					chan.sendMsg(user, RPL_MODE(user, chan, "+i", ""), 1);
					break;
				case 1:
					chan.set_opTopic();
					chan.sendMsg(user, RPL_MODE(user, chan, "+t", ""), 1);
					break;
				case 2:
					if (arg.size() < 4 || arg[3].empty())
						break;
					chan.set_ChannelKey(arg[3]);
					chan.sendMsg(user, RPL_MODE(user, chan, "+k", chan.get_ChannelKey()), 1);
					break;
				case 3: 
					if (arg.size() < 4 || arg[3].empty())
						break;
					for (std::vector<User>::iterator it = chan.get_ChannelUser().begin(); \
						it != chan.get_ChannelUser().end(); ++it)
						if (it->get_nickname() == arg[3]) { 
							chan.set_opChannel(it->get_nickname());
							chan.sendMsg(user, RPL_MODE(user, chan, "+o", arg[3]), 1);
							return;
						}
					server.sendMsg(user, ERR_USERNOTINCHANNEL(user, arg[3], chan), 2);
					break;
				case 4:
					if (arg.size() < 4 || arg[3].empty() || arg[3].find_first_not_of("0123456789") != std::string::npos)
						return;
					if (strtol(arg[3].c_str(), NULL, 10) < 1 || strtol(arg[3].c_str(), NULL, 10) > INT_MAX)
						return;
					chan.set_limitUser(strtol(arg[3].c_str(), NULL, 10));
					chan.sendMsg(user, RPL_MODE(user, chan, "+l", arg[3]), 1);
					break;
			}
			return;
		}
	}
	server.sendMsg(user, ERR_UNKNOWNMODE(user, arg[2]), 2);
}

void	unset_mode(Server &server, User &user, Channel &chan, std::vector<std::string> &arg) {
	char	mode[] = {'i', 't', 'k', 'o', 'l'};

	for (int i = 0; i < 5; i++) {
		if (mode[i] == arg[2][1]) {
			switch (i) {
				case 0:
					chan.unset_inviteOnly();
					chan.delete_inviteList();
					chan.sendMsg(user, RPL_MODE(user, chan, "-i", ""), 1);
					break;
				case 1:
					chan.unset_opTopic();
					chan.sendMsg(user, RPL_MODE(user, chan, "-t", ""), 1);
					break;
				case 2:
					if (chan.get_ChannelKey().empty())
						break;
					chan.sendMsg(user, RPL_MODE(user, chan, "-k", chan.get_ChannelKey()), 1);
					chan.unset_ChannelKey();
					break;
				case 3:
					if (arg.size() < 4 || arg[3].empty())
						break;
					for (std::vector<User>::iterator itu = chan.get_ChannelUser().begin(); \
						itu != chan.get_ChannelUser().end(); ++itu)
						if (itu->get_nickname() == arg[3]) {
							chan.unset_opChannel(itu->get_nickname());
							chan.sendMsg(user, RPL_MODE(user, chan, "-o", arg[3]), 1);
							return;
						}
					server.sendMsg(user, ERR_USERNOTINCHANNEL(user, arg[3], chan), 2);
					break;
				case 4:
					chan.unset_limitUser();
					chan.sendMsg(user, RPL_MODE(user, chan, "-l", ""), 1);
					break;
			}
			return;
		}
	}
	server.sendMsg(user, ERR_UNKNOWNMODE(user, arg[2]), 2);
}

void Commands::mode(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg.size() == 3 && arg[1] == user.get_nickname() && arg[2] == "+i"){
		std::string msg = user.get_nickname() + " MODE " + user.get_nickname() + " :+iH";
		server.sendMsg(user, msg, 2);
	}
	for (std::vector<Channel>::iterator it = server.get_channels().begin(); 
		it != server.get_channels().end(); ++it) {
		if (arg.size() == 2 && it->get_ChannelName() == arg[1])
		{
			std::cout << "MODE " << arg[1] << std::endl;
			return (server.sendMsg(user, RPL_CHANNELMODEIS(user, *it), 1));
		}
		if (arg.size() == 3 && it->get_ChannelName() == arg[1] && arg[2] == "b")
			return (server.sendMsg(user, RPL_ENDOFBANLIST(user, *it), 1));
		if (it->get_ChannelName() == arg[1]) {
			if (it->is_opChannel(user.get_nickname()) == true) {
				if (arg[2][0] == '+')
					set_mode(server, user, *it, arg);
				else if (arg[2][0] == '-')
					unset_mode(server, user, *it, arg);
				else
				 	return(server.sendMsg(user, ERR_UNKNOWNMODE(user, arg[2]), 2));
			}
			else
				server.sendMsg(user, ERR_CHANOPRIVSNEEDED(user, *it), 2);
		}
	}
}
