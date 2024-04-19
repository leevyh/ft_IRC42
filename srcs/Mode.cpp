#include "Commands.hpp"

/*Command: MODE | Parameters: <channel> *( ( "-" / "+" ) *<modes> *<modeparams> )

Numeric Replies: ERR_NEEDMOREPARAMS; ERR_NOCHANMODES; ERR_CHANOPRIVSNEEDED;
				ERR_USERNOTINCHANNEL; ERR_UNKNOWNMODE; RPL_CHANNELMODEIS;
				RPL_INVITELIST; RPL_ENDOFINVITELIST */
void Commands::mode(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg.size() < 3)																// afficher les modes ?
		return;
	for (std::map<std::string, Channel>::iterator it = server.get_channels().begin(); 
		it != server.get_channels().end(); ++it) {
		if (it->first == arg[1]) {
			if (it->second.is_opChannel(user.get_nickname()) == true) {
				std::vector<User> users = it->second.get_UserChannel();
				if (arg[2][0] == '+')
					set_mode(server, user, it->second, arg);
				else if (arg[2][0] == '-')
					unset_mode(server, user, it->second, arg);
				else
				 	return(server.sendMsg(user, ERR_UNKNOWNMODE(user, arg[2]), 2));
			}
			else
				server.sendMsg(user, ERR_CHANOPRIVSNEEDED(user, it->second), 2);
		}
	}
}

void	Commands::set_mode(Server &server, User &user, Channel &chan, std::vector<std::string> &arg) {
	char	mode[] = {'i', 't', 'k', 'o', 'l'};

	for (int i = 0; i < 5; i++) {
		if (mode[i] == arg[2][1]) {
			switch (i) {
				case 0:
					chan.set_inviteOnly();
					chan.sendMsg(user, RPL_MODE(user, chan, "+i", ""), 1);
					break;
				case 1:
					chan.set_optopic();
					chan.sendMsg(user, RPL_MODE(user, chan, "+t", ""), 1);
					break;
				case 2:
					if (arg.size() < 4 || arg[3].empty())
						break;
					chan.set_password(arg[3]);
					chan.sendMsg(user, RPL_MODE(user, chan, "+k", chan.get_password()), 1);
					break;
				case 3: 
					if (arg.size() < 4 || arg[3].empty())
						break;
					for (std::vector<User>::iterator it = chan.get_UserChannel().begin(); \
						it != chan.get_UserChannel().end(); ++it)
						if (it->get_nickname() == arg[3]) { 
							chan.set_opChannel(it->get_nickname());
							chan.sendMsg(user, RPL_MODE(user, chan, "+o", arg[3]), 1);
							return;
						}
					server.sendMsg(user, ERR_USERNOTINCHANNEL(user, arg[3], chan), 2);
					break;
				case 4:
					if (arg.size() < 4 || arg[3].empty() || arg[3].find_first_not_of("0123456789") != std::string::npos)
						break;
					chan.set_limitUser(strtol(arg[3].c_str(), NULL, 10));
					chan.sendMsg(user, RPL_MODE(user, chan, "+l", arg[3]), 1);
					break;
			}
			return;
		}
	}
	server.sendMsg(user, ERR_UNKNOWNMODE(user, arg[2]), 2);
}

void	Commands::unset_mode(Server &server, User &user, Channel &chan, std::vector<std::string> &arg) {
	char	mode[] = {'i', 't', 'k', 'o', 'l'};

	for (int i = 0; i < 5; i++) {
		if (mode[i] == arg[2][1]) {
			switch (i) {
				case 0:
					chan.unset_inviteOnly();
					chan.sendMsg(user, RPL_MODE(user, chan, "-i", ""), 1);
					break;
				case 1:
					chan.unset_optopic();
					chan.sendMsg(user, RPL_MODE(user, chan, "-t", ""), 1);
					break;
				case 2:
					if (chan.get_password().empty())
						break;
					chan.sendMsg(user, RPL_MODE(user, chan, "-k", chan.get_password()), 1);
					chan.unset_password();
					break;
				case 3:
					if (arg.size() < 4 || arg[3].empty())
						break;
					for (std::vector<User>::iterator itu = chan.get_UserChannel().begin(); \
						itu != chan.get_UserChannel().end(); ++itu)
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
