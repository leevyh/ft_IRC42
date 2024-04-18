#include "Commands.hpp"

/*Command: MODE | Parameters: <channel> *( ( "-" / "+" ) *<modes> *<modeparams> )

The MODE command is provided so that users may query and change the
characteristics of a channel.  For more details on available modes
and their uses, see "Internet Relay Chat: Channel Management" [IRC-
CHAN].  Note that there is a maximum limit of three (3) changes per
command for modes that take a parameter.

Numeric Replies: ERR_NEEDMOREPARAMS; ERR_KEYSET;
				ERR_NOCHANMODES; ERR_CHANOPRIVSNEEDED;
				ERR_USERNOTINCHANNEL; ERR_UNKNOWNMODE;
				RPL_CHANNELMODEIS;
				RPL_BANLIST; RPL_ENDOFBANLIST;
				RPL_EXCEPTLIST; RPL_ENDOFEXCEPTLIST;
				RPL_INVITELIST; RPL_ENDOFINVITELIST;
				RPL_UNIQOPIS */
void Commands::mode(Server &server, User &user, std::vector<std::string> &arg) {
	for (std::map<std::string, Channel>::iterator it = server.get_channels().begin(); 
		it != server.get_channels().end(); ++it) {
		if (it->first == arg[1]) {
			Channel chan = it->second;
		
			if (it->second.is_opChannel(user.get_username()) == true) {
				std::vector<User> users = chan.get_UserChannel();
				if (arg[2][0] == '+')
					set_mode(server, user, chan, arg);
				else if (arg[2][0] == '-')
					unset_mode(server, user, chan, arg);
				else
				 	return(server.sendMsg(user, ERR_UNKNOWNMODE(user, arg[2]), 2));
			}
			else
				server.sendMsg(user, ERR_CHANOPRIVSNEEDED(user, chan), 2);
		}
	}
}

void	Commands::set_mode(Server &server, User &user, Channel &chan, std::vector<std::string> &arg) {
	char	mode[] = {'i', 't', 'k', 'o', 'l'};
	int		i = 0;

	while (i < 5) {
		if (mode[i] == arg[2][1]) {
			switch (i) {
				case 0: // i: Set/remove Invite-only channel
					chan.set_inviteOnly();
					chan.sendMsg(RPL_MODE(user, chan, arg[2], ""));
					break;
				case 1: // t: Set/remove the restrictions of the TOPIC command to channel operators
					std::cout << "/MODE +t à coder\n";
					break;
				case 2: // k: Set/remove the channel key (password)
					if (!arg[3].compare(user.get_ip()))
						break;
					chan.set_password(arg[3]);
					chan.sendMsg(RPL_MODE(user, chan, arg[2], chan.get_password()));
					break;
				case 3: // +o: Give channel operator privilege
					for (std::vector<User>::iterator itu = chan.get_UserChannel().begin(); 
						itu != chan.get_UserChannel().end(); ++itu)
						if (itu->get_username() == arg[3]) {
							chan.set_opChannel(itu->get_username());
							server.get_channels()[chan.get_ChannelName()] = chan;
							chan.sendMsg(RPL_MODE(user, chan, arg[2], arg[3]));
							break;
						}
					server.sendMsg(user, ERR_USERNOTINCHANNEL(user, arg[3], chan), 2);
					break;
				case 4: // l: Set/remove the user limit to channel
					if (arg.size() < 4 || arg[3].empty() || arg[3].find_first_not_of("0123456789") != std::string::npos)
						break;
					chan.set_limitUser(strtol(arg[3].c_str(), NULL, 10));
					chan.sendMsg(RPL_MODE(user, chan, arg[2], arg[3]));
					break;
				default:
					server.sendMsg(user, ERR_UNKNOWNMODE(user, arg[2]), 2);
					break;
			}
		}
		i++;
	}
}

void	Commands::unset_mode(Server &server, User &user, Channel &chan, std::vector<std::string> &arg) {
	char	mode[] = {'i', 't', 'k', 'o', 'l'};
	int		i = 0;

	while (i < 5 ) {
		if (mode[i] == arg[2][1]) {
			switch (i) {
				case 0: // i: Set/remove Invite-only channel
					chan.unset_inviteOnly();
					chan.sendMsg(RPL_MODE(user, chan, arg[2], ""));
					break;
				case 1: // t: Set/remove the restrictions of the TOPIC command to channel operators
					std::cout << "/MODE -t à coder\n";
					break;
				case 2: // k: Set/remove the channel key (password)
					chan.sendMsg(RPL_MODE(user, chan, arg[2], chan.get_password()));
					chan.unset_password();
					break;
				case 3: // o: Give channel operator privilege
					for (std::vector<User>::iterator itu = chan.get_UserChannel().begin(); 
						itu != chan.get_UserChannel().end(); ++itu)
						if (itu->get_nickname() == arg[3]) {
							chan.unset_opChannel(itu->get_nickname());
							server.get_channels()[chan.get_ChannelName()] = chan;
							chan.sendMsg(RPL_MODE(user, chan, arg[2], arg[3]));
							break;
						}
					server.sendMsg(user, ERR_USERNOTINCHANNEL(user, arg[3], chan), 2);
					break;
				case 4: // l: Set/remove the user limit to channel
					chan.unset_limitUser();
					chan.sendMsg(RPL_MODE(user, chan, arg[2], ""));
					break;
				default:
					server.sendMsg(user, ERR_UNKNOWNMODE(user, arg[2]), 2);
					break;
			}
		}
		i++;
	}
}
