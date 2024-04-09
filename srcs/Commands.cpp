#include "Commands.hpp"

Commands::Commands(void) {
	cmdMap["NICK"] = &Commands::nick;
	cmdMap["PASS"] = &Commands::pass;
	cmdMap["USER"] = &Commands::user;
	cmdMap["CAP"] = &Commands::capls;
}

Commands::~Commands(void) {
}

void Commands::getcommand(Server &server, User &user, std::vector <std::string> &argument) {

// 	// debug("getCommand", BEGIN);
	bool command = false;
	// if ((!argument.empty() && argument.size() > 1)) {
	// 	if (argument[0] == "CAP" && argument[1] == "END") {
	// 		displayWelcome(server, user);
	// 		server.setIrssi(false);
	// 		return;
	// 	}
	// }

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

/* Command PASS | Parameters: <password>

The PASS command is used to set a 'connection password'.
The optional password can and MUST be set before any attempt to 
register the connection is made. Currently this requires that user 
send a PASS command before sending the NICK/USER combination.

Numeric Replies: ERR_NEEDMOREPARAMS; ERR_ALREADYREGISTRED */
void Commands::pass( Server& server, User& user, std::vector<std::string>& arg ) {
	if (user.get_status() == true)
		return (server.sendMsg(user, ERR_ALREADYREGISTRED(user)));;
	if (arg[1].empty())
		return (server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "PASS")));
	if (arg[1] == server.get_Password()) {
		if (user.get_password().empty())
			user.set_password(arg[1]);
		else
			return (server.sendMsg(user, ERR_ALREADYREGISTRED(user)));
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
void Commands::nick( Server& server, User& user, std::vector<std::string>& arg ) {
	if (arg[1].empty())
		return (server.sendMsg(user, ERR_NONICKNAMEGIVEN(arg[1])));
	if (arg[1].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != std::string::npos) //ajouter les autres caractères autorisés
		return (server.sendMsg(user, ERR_ERRONEUSNICKNAME(arg[1])));

	std::map<std::string, User*> users = server.get_usersbynick();
	std::map<std::string, User*>::iterator it;
	it = users.find(arg[1]);
	if (it != users.end())
		return (server.sendMsg(user, ERR_NICKNAMEINUSE(arg[1])));
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
void Commands::user( Server& server, User& user, std::vector<std::string>& arg ) {
//			>> USER <user> <mode> <unused> <realname>
//Example:	>> USER amugnier amugnier 10.24.4.2 :Antoine MUGNIER
	if (user.get_status() == true)
		return (server.sendMsg(user, ERR_ALREADYREGISTRED(user)));
	if (arg.size() < 5)
		return (server.sendMsg(user, ERR_NEEDMOREPARAMS(user, "USER")));
	user.set_username(arg[1]);
	// if (arg[2].size()) // Should be numeric ??
	// 	user.set_mode();
	user.set_ip(arg[3]);
	if (arg[4][0] == ':') {
		std::string realname;
		arg[4] = arg[4].erase(0,1);
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

A client session is terminated with a quit message.  The server
acknowledges this by sending an ERROR message to the client.

Numeric Replies: None. */
void quit();
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
void join();



void Commands::capls( Server& server, User& user, std::vector<std::string>& arg ) {
	(void)server;
	(void)user;
	(void)arg;
	std::cout << "CAP LS\n";
}
