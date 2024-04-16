#include "Commands.hpp"

Commands::Commands(void) {
	cmdMap["CAP"] = &Commands::capls;
	cmdMap["PASS"] = &Commands::pass;
	cmdMap["NICK"] = &Commands::nick;
	cmdMap["USER"] = &Commands::user;
	cmdMap["QUIT"] = &Commands::quit;
	cmdMap["MODE"] = &Commands::mode;
	cmdMap["PING"] = &Commands::ping;
	cmdMap["PRIVMSG"] = &Commands::privmsg;
	cmdMap["JOIN"] = &Commands::join;
}

Commands::~Commands(void) {
}

void Commands::getcommand(Server &server, User &user, std::vector<std::string> &argument) {

// 	// debug("getCommand", BEGIN);
	bool command = false;

// Check si la commande fait partie de notre liste de commandes
	if (!argument.empty()) {
		for (std::map<std::string, cmdFPtr>::iterator it = cmdMap.begin(); it != cmdMap.end(); ++it) {
			if (it->first == argument[0]) {
				(this->*(it->second))(server, user, argument);
				command = true;
			}
		}
		if (command == false)
			server.sendMsg(user, ERR_UNKNOWNCOMMAND(user, argument[0]));
	}
	else
		server.sendMsg(user, ERR_UNKNOWNCOMMAND(user, ""));
	return;
}


void Commands::capls(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg.size() > 1 && arg[1] == "LS") {
		server.sendMsg(user, "CAP * LS :none");
		std::cout << "CAP * LS :none\n";
		user.set_status(true);
		server.set_Irssi(true);
	}
	if (arg.size() > 1 && arg[1] == "END") {
		displayWelcome(server, user);
		server.set_Irssi(false);
	}
}


/* Command PASS | Parameters: <password>

The PASS command is used to set a 'connection password'.
The optional password can and MUST be set before any attempt to 
register the connection is made. Currently this requires that user 
send a PASS command before sending the NICK/USER combination.

Numeric Replies: ERR_NEEDMOREPARAMS; ERR_ALREADYREGISTRED */
void Commands::pass(Server &server, User &user, std::vector<std::string> &arg) {
	if (user.get_status() == true) {
		return (server.sendMsg(user, ERR_ALREADYREGISTRED(user)));
	};
	if (arg[1].empty()) {
		return (server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "PASS")));
	}
	if (arg[1] == server.get_Password()) {
		if (user.get_password().empty()) {
			user.set_password(arg[1]);
		}
		else {
			return (server.sendMsg(user, ERR_ALREADYREGISTRED(user)));
		}
	}
}

/* Command NICK | Parameters: <nickname>

NICK command is used to give user a nickname or change the existing one.

Nicknames are non-empty strings with the following restrictions:

They MUST NOT contain any of the following characters: space (' ', 0x20), comma (',', 0x2C), 
asterisk ('*', 0x2A), question mark ('?', 0x3F), exclamation mark ('!', 0x21), at sign ('@', 0x40).
They MUST NOT start with any of the following characters: dollar ('$', 0x24), colon (':', 0x3A).
They MUST NOT start with a character listed as a channel type, channel membership prefix, 
or prefix listed in the IRCv3 multi-prefix Extension.
They SHOULD NOT contain any dot character ('.', 0x2E).

Servers MAY have additional implementation-specific nickname restrictions and SHOULD avoid 
the use of nicknames which are ambiguous with commands or command parameters 
where this could lead to confusion or error.

Numeric Replies: ERR_NONICKNAMEGIVEN; ERR_ERRONEUSNICKNAME;
				ERR_NICKNAMEINUSE; ERR_NICKCOLLISION;
				ERR_UNAVAILRESOURCE; ERR_RESTRICTED */
void Commands::nick(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg[1].empty()) {
		return (server.sendMsg(user, ERR_NONICKNAMEGIVEN(arg[1])));
	}
	std::cout << "user->nickname: " << arg[1] << std::endl;
	if (arg[1].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") \
		!= std::string::npos) { //ajouter les autres caractères autorisés
		return (server.sendMsg(user, ERR_ERRONEUSNICKNAME(arg[1])));
	}
	std::map<int, User> users = server.get_clientmap();
	for(std::map<int, User>::iterator it = server.get_clientmap().begin(); \
		it != server.get_clientmap().end(); ++it) {
		if (arg[1] == it->second.get_nickname()) {
			server.sendMsg(user, ERR_NICKNAMEINUSE(arg[1]));
	
			// // Est-ce que le server lui attribue un nouveau NICK ?
			// int i = 48;
			// std::string new_nick = arg[1] + (char)i;
			// for(it = server.get_clientmap().begin(); it != server.get_clientmap().end(); ++it) {
			// 	if (new_nick == it->second.get_nickname()) {
			// 		server.sendMsg(user, ERR_NICKNAMEINUSE(new_nick));
			// 		i++;
			// 		new_nick = arg[1]+ (char)i;
			// 	}
			// 	else {
			// 		server.sendMsg(user, arg[1] + " changed his nickname to " + new_nick);
			// 		std::string resp1 = ":" + arg[1] + "!~" + arg[1] + "@" + user.get_ip() \
			// 		+ " NICK :" + new_nick + "\r\n";
			// 		if (send(user.get_fd(), resp1.c_str(), resp1.length(), 0) == -1)
			// 			std::perror("send:");
			// 	}
			// }

		}
	}
	if (!user.get_nickname().empty()) {
		server.sendMsg(user, user.get_nickname() + " changed his nickname to " + arg[1]);
		std::string resp = ":" + user.get_nickname() + "!~" + user.get_username() + "@" + user.get_ip() \
		+ " NICK :" + arg[1] + "\r\n";
		if (send(user.get_fd(), resp.c_str(), resp.length(), 0) == -1)
			std::perror("send:");
	}
	user.set_nickname(arg[1]);
};

/* Command USER | Parameters: <user> <mode> <unused> <realname>

The USER command is used at the beginning of connection to specify
the username, hostname and realname of a new user.
The <mode> parameter should be a numeric, and can be used to
automatically set user modes when registering with the server. This
parameter is a bitmask, with only 2 bits having any signification: if
the bit 2 is set, the user mode 'w' will be set and if the bit 3 is
set, the user mode 'i' will be set. (See Section 3.1.5 "User Modes").
The <realname> may contain space characters.

Numeric Replies: ERR_NEEDMOREPARAMS; ERR_ALREADYREGISTRED */
void Commands::user(Server &server, User &user, std::vector<std::string> &arg) {
//			>> USER <user> <mode> <unused> <realname>
//Example:	>> USER amugnier amugnier 10.24.4.2 :Antoine MUGNIER
//	if (user.get_status() == true) {
//		return (server.sendMsg(user, ERR_ALREADYREGISTRED(user)));
//	} //TODO TEMPORAIRE EN COMMENTAIRE POUR TESTER LES MP
	if (arg.size() < 5) {
		return (server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "USER")));
	}
	user.set_username(arg[1]);
	// if (arg[2].size()) // Should be numeric ??
	// 	user.set_mode();
	user.set_ip(arg[3]);
	std::cout << "user->ip: " << arg[3] << std::endl;
	if (arg[4][0] == ':') {
		std::string realname;
		arg[4] = arg[4].erase(0, 1);
		for (size_t i = 4; i < arg.size(); i++) {
			realname += arg[i];
			realname += " ";
		}
		user.set_realname(realname);
	}
	else {
		user.set_realname(arg[4]);
	}
}


/* Command: QUIT | Parameters: [ <Quit Message> ]

A client session is terminated with a quit message.  The server
acknowledges this by sending an ERROR message to the client.

If a client connection is closed without the client issuing a QUIT command 
to the server, the server MUST distribute a QUIT message to other clients informing 
them of this, distributed in the same was an ordinary QUIT message. Servers MUST 
fill <reason> with a message reflecting the nature of the event which caused it to happen. 
For instance, "Ping timeout: 120 seconds", "Excess Flood", and "Too many connections from 
this IP" are examples of relevant reasons for closing or for a connection with a client 
to have been closed.

Numeric Replies: None. */
void Commands::quit(Server &server, User &user, std::vector<std::string> &arg) {
// Command Example:   QUIT :Gone to have lunch         ; Client exiting from the network
// Message Example:  :dan-!d@localhost QUIT :Quit: Bye for now!
// 					; dan- is exiting the network with the message: "Quit: Bye for now!"
	(void)server;
	(void)user;
	(void)arg;
	std::cout << "QUIT à coder\n";
}


/* Command: JOIN | Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"

The JOIN command is used by a user to request to start listening to
the specific channel.  Servers MUST be able to parse arguments in the
form of a list of target, but SHOULD NOT use lists when sending JOIN
messages to clients.

Once a user has joined a channel, he receives information about
all commands his server receives affecting the channel.  This
includes JOIN, MODE, KICK, PART, QUIT and of course PRIVMSG/NOTICE.
This allows channel members to keep track of the other channel
members, as well as channel modes.

If a JOIN is successful, the user receives a JOIN message as
confirmation and is then sent the channel's topic (using RPL_TOPIC) and
the list of users who are on the channel (using RPL_NAMREPLY), which
MUST include the user joining.

Note that this message accepts a special argument ("0"), which is
a special request to leave all channels the user is currently a member
of. The server will process this message as if the user had sent
a PART command (See Section 3.2.2) for each channel he is a member of.

Numeric Replies: ERR_NEEDMOREPARAMS; ERR_BANNEDFROMCHAN;
				ERR_INVITEONLYCHAN; ERR_BADCHANNELKEY;
				ERR_CHANNELISFULL; ERR_BADCHANMASK;
				ERR_NOSUCHCHANNEL; ERR_TOOMANYCHANNELS;
				ERR_TOOMANYTARGETS; ERR_UNAVAILRESOURCE;
				RPL_TOPIC */

std::vector<std::string> split(const std::string& str)
{
	std::vector<std::string> channels_result;
	std::string delimiter = ",";
	char *args = strtok((char *)str.c_str(), ",");
	while (args != NULL)
	{
		channels_result.push_back(args);
		args = strtok(NULL, ",");
	}
	return (channels_result);
}

int	check_channelName(Server &server, User &user, std::vector<std::string> &channel)
{
	(void)server;
	(void)user;
	for (size_t i = 0; i < channel.size(); i++)
	{
		if (channel[i][0] != '#')
		{
//			server.sendMsg(user, ERR_BADCHANMASK(user, channel[i]));
			std::cout << "ERR_BADCHANMASK aa coder" << std::endl;
			return (-6969);
		}
		if (channel[i].find_first_not_of("#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") !=
			std::string::npos) {
//			server.sendMsg(user, ERR_BADCHANMASK(user, channel[i]));
			std::cout << "ERR_BADCHANMASK a coder" << std::endl;
			return (-6969);
		}
	}
	return (0);
}

void Commands::join(Server &server, User &user, std::vector<std::string> &arg) {
	std::vector<std::string> channels;
	channels = split(arg[1]);
	std::string msg_send;
	if (check_channelName(server, user, channels) == -6969)
		return;
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (server.get_channels().empty()) {
			Channel channel(channels[i]);
			std::cout << "Channel created: " << channel.get_ChannelName() << std::endl;
			channel.set_UserChannel(user);
			std::vector<User> user_list = channel.get_UserChannel();
			for (std::vector<User>::iterator it = user_list.begin(); it != user_list.end(); ++it)
			{
				std::cout << "User in channel: " << it->get_nickname() << std::endl;
			}
			server.get_channels()[channels[i]] = channel;
			msg_send = ":" + user.get_nickname() + "!~" + user.get_username() \
			+ "@localhost.ip JOIN :" + channels[i] + "\r\n";
			std::cout << "Message sent: " << msg_send << std::endl;
//			server.sendMsg(user, msg_send);
			send(user.get_fd(), msg_send.c_str(), msg_send.length(), 0);
		}
		else
		{
			for (std::map<std::string, Channel>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); ++it)
			{
				if (channels[i] == it->second.get_ChannelName())
				{
					std::cout << "Find :" << channels[i] << std::endl;
					it->second.set_UserChannel(user);
					std::vector<User> user_list = it->second.get_UserChannel();
					for (std::vector<User>::iterator it = user_list.begin(); it != user_list.end(); ++it)
					{
						std::cout << "User in channel: " << it->get_nickname() << std::endl;
					}
					msg_send = ":" + user.get_nickname() + "!~" + user.get_username() + "@localhost.ip JOIN :" + channels[i] +
							   "\r\n";
					send(user.get_fd(), msg_send.c_str(), msg_send.length(), 0);
				}
			}
		}
	}
	return;
}


void Commands::mode(Server &server, User &user, std::vector<std::string> &arg) {
	(void)server;
	(void)user;
	(void)arg;
	std::cout << "MODE à coder\n";
}




/* Command: PING | Parameters: <server1> [ <server2> ]

The PING command is used to test the presence of an active client or
server at the other end of the connection.  Servers send a PING
message at regular intervals if no other activity detected coming
from a connection.  If a connection fails to respond to a PING
message within a set amount of time, that connection is closed.  A
PING message MAY be sent even if the connection is active.

When a PING message is received, the appropriate PONG message MUST be
sent as reply to <server1> (server which sent the PING message out)
as soon as possible.  If the <server2> parameter is specified, it
represents the target of the ping, and the message gets forwarded
there.

Numeric Replies: ERR_NOORIGIN (409); ERR_NOSUCHSERVER (402)*/
void Commands::ping(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg[1].empty() || !arg[1].size())
		return (server.sendMsg(user, ERR_NOORIGIN(user)));
	std::string msg_send = "PONG :" + arg[1];
	send(user.get_fd(), msg_send.c_str(), msg_send.length(), 0);
}

/* Command: PONG | Parameters: <server> [ <server2> ]

PONG message is a reply to ping message.  If parameter <server2> is
given, this message MUST be forwarded to given target.  The <server>
parameter is the name of the entity who has responded to PING message
and generated this message.

Numeric Replies: ERR_NOORIGIN (409); ERR_NOSUCHSERVER (402)*/
void Commands::pong(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg[1].empty() || !arg[1].size())
		return (server.sendMsg(user, ERR_NOORIGIN(user)));
	user.set_lastping(time(NULL));
}


/* Command: PRIVMSG | Parameters: <target>{,<target>} <text to be sent>

The PRIVMSG command is used to send private messages between users, 
as well as to send messages to channels. <target> is the nickname of a 
client or the name of a channel.

If <target> is a channel name and the client is banned and not covered by 
a ban exception, the message will not be delivered and the command will silently fail. 
Channels with the moderated mode active may block messages from certain users. 
Other channel modes may affect the delivery of the message or cause the message 
to be modified before delivery, and these modes are defined by the server software 
and configuration being used.

If a message cannot be delivered to a channel, the server SHOULD respond with 
an ERR_CANNOTSENDTOCHAN (404) numeric to let the user know that this message could not be delivered.

If <target> is a channel name, it may be prefixed with one or more channel membership 
prefix character (@, +, etc) and the message will be delivered only to the members of 
that channel with the given or higher status in the channel. Servers that support this feature 
will list the prefixes which this is supported for in the STATUSMSG RPL_ISUPPORT parameter, 
and this SHOULD NOT be attempted by clients unless the prefix has been advertised in this token.

If <target> is a user and that user has been set as away, the server may reply with 
an RPL_AWAY (301) numeric and the command will continue.

The PRIVMSG message is sent from the server to client to deliver a message to that client. 
The <source> of the message represents the user or server that sent the message, and the <target> 
represents the target of that PRIVMSG (which may be the client, a channel, etc).

When the PRIVMSG message is sent from a server to a client and <target> starts with a dollar 
character ('$', 0x24), the message is a broadcast sent to all clients on one or multiple servers.

Numeric Replies: ERR_NOSUCHNICK (401); ERR_NOSUCHSERVER (402);
					ERR_CANNOTSENDTOCHAN (404); ERR_TOOMANYTARGETS (407);
					ERR_NORECIPIENT (411); ERR_NOTEXTTOSEND (412);
					ERR_NOTOPLEVEL (413); ERR_WILDTOPLEVEL (414);
					RPL_AWAY (301)*/
void Commands::privmsg(Server &server, User &user, std::vector<std::string> &arg) {
	std::string full_msg;
	int i = 2;
//	if (arg.size() < 3) {
//		return (server.sendMsg(user, ERR_NORECIPIENT(user)));
//	}
	if (arg[1][0] != '#') {
		for (std::vector<std::string>::iterator it = arg.begin() + 2; it != arg.end(); ++it, i++) {
			if (arg[i][0] == ':')
				full_msg += arg[i].erase(0, 1);
			else
				full_msg += " " + *it;
		}
		for (std::map<int, User>::iterator it = server.get_clientmap().begin();
			 it != server.get_clientmap().end(); ++it) {
			if (arg[1] == it->second.get_nickname()) {
				std::string msg_send =
						":" + user.get_nickname() + "!~" + user.get_username() + "@" + user.get_ip() + " PRIVMSG " +
						arg[1] + " :" + full_msg + "\r\n";
//				server.sendMsg(it->second, msg_send);
				send(it->second.get_fd(), msg_send.c_str(), msg_send.length(),
					 0); //TODO avoir avec livia pour le networkname
//				std::cout << "Message sent to " << it->second.get_nickname() << std::endl;
				return;
			}
		}
		server.sendMsg(user, ERR_NOSUCHNICK(user, arg[1]));
	}
	else {
		for (std::vector<std::string>::iterator it = arg.begin() + 2;
			 it != arg.end(); ++it, i++) //TODO FAIRE UNE FONCTION CAR DOUBLON VOIR PLUS
		{
			if (arg[i][0] == ':')
				full_msg += arg[i].erase(0, 1);
			else
				full_msg += " " + *it;
		}
		for (std::map<std::string, Channel>::iterator it = server.get_channels().begin();
			 it != server.get_channels().end(); it++) {
			std::cout << "Channel: " << it->first << std::endl;
			std::cout << "Channel: " << it->second.get_ChannelName() << std::endl;
			std::cout << "Arg 1: " << arg[1] << std::endl;
			if (arg[1] == it->second.get_ChannelName()) {
				std::vector<User> user_list = it->second.get_UserChannel();
				std::cout <<"User list size: " << user_list.size() << std::endl;
				for (std::vector<User>::iterator ita = user_list.begin(); ita != user_list.end(); ++ita) {
					std::cout << "ita_fd: " << ita->get_fd() << std::endl;
					std::cout << "User fd: " << user.get_fd() << std::endl;
					if (ita->get_fd() == user.get_fd()) {
						for (std::vector<User>::iterator itb = user_list.begin(); itb != user_list.end(); ++itb) {
							if (itb->get_fd() != user.get_fd()) {
								std::cout << "Itb_fd: " << itb->get_fd() << std::endl;
								std::cout << "User fd: " << user.get_fd() << std::endl;
								std::string msg_send =
										":" + user.get_nickname() + "!~" + user.get_username() + "@" + user.get_ip() +
										" PRIVMSG " +
										arg[1] + " :" + full_msg + "\r\n";
								send(itb->get_fd(), msg_send.c_str(), msg_send.length(), 0);
							}
						}
						return;
					}
				}
				//user not in the channel
			}
		}
		//NO SUCH CHANNEL
	}
}