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
	if (arg.size() > 1 && arg[1] == "LS") {
		server.sendMsg(user, "CAP * LS :none", 1);
		// server.set_Irssi(true);
	}
	if (arg.size() > 1 && arg[1] == "END") {
		// displayWelcome(server, user);
		// server.set_Irssi(false);
	}
}


/* Command PASS | Parameters: <password>

The PASS command is used to set a 'connection password'.
The optional password can and MUST be set before any attempt to 
register the connection is made. Currently this requires that user 
send a PASS command before sending the NICK/USER combination.

Numeric Replies: ERR_NEEDMOREPARAMS (461); ERR_ALREADYREGISTRED (462)*/
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

Numeric Replies: ERR_NONICKNAMEGIVEN (431); ERR_ERRONEUSNICKNAME (432);
				ERR_NICKNAMEINUSE (433); ERR_NICKCOLLISION (436);
				ERR_UNAVAILRESOURCE (437); ERR_RESTRICTED (484)*/
void Commands::nick(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg[1].empty()) {
		return (server.sendMsg(user, ERR_NONICKNAMEGIVEN(arg[1]), 1));
	}
	if (arg[1].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") \
		!= std::string::npos) { //ajouter les autres caractères autorisés
		return (server.sendMsg(user, ERR_ERRONEUSNICKNAME(arg[1]), 1));
	}
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

Numeric Replies: ERR_NEEDMOREPARAMS (461); ERR_ALREADYREGISTRED (462)*/
void Commands::user(Server &server, User &user, std::vector<std::string> &arg) {
//			>> USER <user> <mode> <unused> <realname>
//Example:	>> USER amugnier amugnier 10.24.4.2 :Antoine MUGNIER
	if (user.get_authenticated() == true)
		return (server.sendMsg(user, ERR_ALREADYREGISTRED(user), 1));
	if (arg.size() < 5)
		return (server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "USER"), 1));
	user.set_username(arg[1]);
	// if (arg[2].size()) // Should be numeric ??
	// 	user.set_mode();
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
					it->second.sendMsg(final_message);
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

Numeric Replies: ERR_NEEDMOREPARAMS (461); ERR_BANNEDFROMCHAN (474);
				ERR_INVITEONLYCHAN (473); ERR_BADCHANNELKEY (475);
				ERR_CHANNELISFULL (471); ERR_BADCHANMASK (476);
				ERR_NOSUCHCHANNEL (403); ERR_TOOMANYCHANNELS (405);
				ERR_TOOMANYTARGETS (407); ERR_UNAVAILRESOURCE;
				RPL_TOPIC (332) */

void	channel_BroadcastJoin(Server &server, User &user, std::string const &channel_name)
{
	std::string msg_send;
	std::vector<User> user_list = server.get_channels()[channel_name].get_UserChannel();
	for (std::vector<User>::iterator it = user_list.begin(); it != user_list.end(); ++it)
	{
		if (it->get_fd() != user.get_fd())
		{
			msg_send = user.get_nickname() + "!~" + user.get_username() + "@localhost.ip JOIN :" + channel_name;
			server.sendMsg(*it, msg_send, 2);
		}
	}
}


short is_Authorize(Server &server, User &user, Channel &Channel, std::vector<std::string> &key, size_t i)
{
	(void)server;
	(void)user;
	if (Channel.is_ValidKey(key, i) == false)
		return (2);
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
	for(size_t i = 0; i < channels.size(); i++)
	{
		if (!(server.get_channels().empty()))
		{
			authorized = 0;
			for (std::map<std::string, Channel>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); ++it)
			{
				if (channels[i] == it->second.get_ChannelName())
				{
					authorized = is_Authorize(server, user, it->second, key, i);
					switch (authorized) {
						case 1:
							add_UserInChannel(server, user, it->second);
							channel_BroadcastJoin(server, user, channels[i]);
							if (!it->second.get_ChannelTopic().empty())
								server.sendMsg(user, RPL_TOPIC(user, it->second), 1);
							server.sendMsg(user, RPL_NAMES(user, it->second), 1);
							server.sendMsg(user, RPL_ENDOFNAMES(user, it->second), 1);
							server.sendMsg(user, RPL_CREATIONTIME(user, it->second), 1);
							break;
						case 2:
							server.sendMsg(user, ERR_BADCHANNELKEY(user, it->second), 1);
							break;
						default:
							std::cerr << "Error: Unknown error" << std::endl;
					}
//						case 3:
//							std::cout << "ERR_CHANNELISFULL a coder" << std::endl;
//							break;
//						case 4:
//							std::cout << "ERR_INVITEONLYCHAN a coder" << std::endl;
//							break;
				}
			}
		}
		if (authorized == 0)
			create_NewChannel(server, user, channels[i]);
	}
	return;
}

std::vector<std::string> split(const std::string& str)
{
	std::vector<std::string> channels_result;
	std::string delimiter = ",";
	char *args = strtok((char *)str.c_str(), ",");
	while (args != NULL) {
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
		if (channel[i].find_first_not_of("#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") 
			!= std::string::npos) {
			server.sendMsg(user, ERR_BADCHANMASK(channel[i]), 1);
			// std::cout << "ERR_BADCHANMASK a coder" << std::endl;
			return (-6969);
		}
	}
	return (0);
}

void	create_NewChannel(Server &server, User &user, std::string const &channel_name)
{
	Channel channel(channel_name);
	channel.set_ChannelUser(user);
	channel.set_opChannel(user.get_nickname());
	channel.creationTime();
	server.get_channels()[channel_name] = channel;
	server.sendMsg(user, RPL_JOIN(user, channel), 2);
	server.sendMsg(user, RPL_NAMES(user, channel), 1);
	server.sendMsg(user, RPL_ENDOFNAMES(user, channel), 1);
	server.sendMsg(user, RPL_CREATIONTIME(user, channel), 1);
}

void	add_UserInChannel(Server &server, User &user, Channel &channel)
{
	channel.set_ChannelUser(user);
  server.sendMsg(user, RPL_JOIN(user, channel), 2);
	//			>> :atw.hu.eu.dal.net 353 Neoblack_ = #bleu :Neoblack_ @Neoblack
//					>> :atw.hu.eu.dal.net 366 Neoblack_ #bleu :End of /NAMES list.
}


/* --------------------------------------------------END JOIN-------------------------------------------------- */

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


/* Command: PING | Parameters: <token>

The PING command is sent by either clients or servers to check the other side of
the connection is still connected and/or to check for connection latency,
at the application layer.

The <token> may be any non-empty string.
When receiving a PING message, clients or servers must reply to it with a PONG
message with the same <token> value. This allows either to match PONG with the
PING they reply to, for example to compute latency.

Clients should not send PING during connection registration, though servers may 
accept it. Servers may send PING during connection registration and clients must 
reply to them.

Older versions of the protocol gave specific semantics to the <token> and allowed 
an extra parameter; but these features are not consistently implemented and 
should not be relied on. Instead, the <token> should be treated as an opaque value 
by the receiver.

Numeric Replies: ERR_NOORIGIN (409); ERR_NOSUCHSERVER (402)*/
void Commands::ping(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg[1].empty() || !arg[1].size())
		return (server.sendMsg(user, ERR_NOORIGIN(user), 1));
	std::string msg_send = "PONG :" + arg[1];
	server.sendMsg(user, msg_send, 1);
}


/* Command: PONG | Parameters: [<server>] <token>
The PONG command is used as a reply to PING commands, by both clients and servers. 
The <token> should be the same as the one in the PING message that triggered this PONG.

Servers MUST send a <server> parameter, and clients SHOULD ignore it. It exists for 
historical reasons, and indicates the name of the server sending the PONG. Clients 
MUST NOT send a <server> parameter.

Numeric Replies: none*/
void Commands::pong(Server &server, User &user, std::vector<std::string> &arg) {
	if (arg[1].empty() || !arg[1].size())
		return (server.sendMsg(user, ERR_NOORIGIN(user), 1));

	// CHECK si le token recu correspond a ce qu'on a envoyé == STOCKER celui envoyé ?
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
	if (arg.size() < 3) {
		return (server.sendMsg(user, ERR_NORECIPIENT(user, "PRIVMSG"), 1));
	}
	if (arg[1][0] != '#') {
		full_msg = remove_OneChar(':', arg);
		for (std::map<int, User>::iterator it = server.get_clientmap().begin();
			 it != server.get_clientmap().end(); ++it) {
			if (arg[1] == it->second.get_nickname()) {
				std::string msg_send =
						":" + user.get_nickname() + "!~" + user.get_username() + "@" + user.get_ip() + " PRIVMSG " +
						arg[1] + " :" + full_msg;
				server.sendMsg(it->second, msg_send, 2);
				return;
			}
		}
		server.sendMsg(user, ERR_NOSUCHNICK(user, arg[1]), 1);
	}
	else {
		full_msg = remove_OneChar(':', arg);
		for (std::map<std::string, Channel>::iterator it = server.get_channels().begin();
			 it != server.get_channels().end(); it++)
			if (arg[1] == it->second.get_ChannelName()) {
				std::vector<User> user_list = it->second.get_UserChannel();
				for (std::vector<User>::iterator ita = user_list.begin(); ita != user_list.end(); ++ita) {
					if (ita->get_fd() == user.get_fd()) {
						for (std::vector<User>::iterator itb = user_list.begin(); itb != user_list.end(); ++itb) {
							if (itb->get_fd() != user.get_fd()) {
								std::string msg_send =
										":" + user.get_nickname() + "!~" + user.get_username() + "@" + user.get_ip() +
										" PRIVMSG " +
										arg[1] + " :" + full_msg;
								server.sendMsg(*itb, msg_send, 2);
							}
						}
						return;
					}
				}
			}
		server.sendMsg(user, ERR_CANNOTSENDTOCHAN(user, arg[1]), 1);
	}
}
