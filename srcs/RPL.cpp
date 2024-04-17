#include "IRC.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "Commands.hpp"

void displayWelcome(Server &server, User &user) {
	server.sendMsg(user, RPL_WELCOME(server, user), 1);
	server.sendMsg(user, RPL_YOURHOST(server, user), 1);
	server.sendMsg(user, RPL_CREATED(user), 1);
	server.sendMsg(user, RPL_MYINFO(server, user), 1);
}

std::string RPL_WELCOME(Server &server, User &user) {
	return ("001 " + user.get_nickname() + " :Welcome to the "
			+ server.get_networkname() + " Network, " + user.get_nickname()
			+ "!~" + user.get_username() + "@" + user.get_ip());
}

std::string RPL_YOURHOST(Server &server, User &user) {
	return ("002 " + user.get_nickname() + " :Your host is "
			+ server.get_networkname() + ", Network, running v0.1");
}

std::string RPL_CREATED(User &user) {
	return ("003 " + user.get_nickname() + " :This server was created Sun 20 Apr 2042 during the total eclipse");
}

// "<client> <servername> <version> <available user modes>
// <available channel modes> [<channel modes with a parameter>]"
std::string RPL_MYINFO(Server &server, User &user) {
	return ("004 " + user.get_nickname() + " " + server.get_networkname() + \
	" v0.1 <available user modes> <available channel modes>");
}

/* ************************************************************************** */

// Indicates that the user with the nickname <nick> is currently away and sends the away message that they set.
// "<client> <nick> :<message>"
std::string RPL_AWAY(User &user, std::string recipient, std::string away_msg) {
	return ("301 " + user.get_username() + " " + recipient + " :" + away_msg);
}

std::string RPL_TOPIC(User &user, std::string channel, std::string topic) {
	return ("332 " + user.get_username() + " " + channel + " :" + topic);
}

std::string ERR_NOSUCHNICK(User &user, std::string nickname) {
	return ("401 " + user.get_username() + " " + nickname + " :No suck nick/channel");
}

std::string ERR_NOSUCHSERVER(User &user, std::string server_name) {
	return ("402 " + user.get_username() + " " + server_name + " :No such server");
}

std::string ERR_NOSUCHCHANNEL(User &user, std::string channel) {
	return ("403 " + user.get_username() + " " + channel + " :No such channel");
}

std::string ERR_CANNOTSENDTOCHAN(User &user, std::string channel) {
	return ("404 " + user.get_username() + " " + channel + " :Cannot send to channel");
}

std::string ERR_NOORIGIN(User &user) {
	return ("409 " + user.get_username() + ":No origin specified");
}

// Returned by the PRIVMSG command to indicate the message wasn’t delivered because there was no recipient given. -> Recipient = destinataire
std::string ERR_NORECIPIENT(User &user, std::string command) {
	return ("411 " + user.get_username() + " :No recipient given (" + command + ")");
}

// Returned by the PRIVMSG command to indicate the message wasn’t delivered because there was no text to send.
std::string ERR_NOTEXTTOSEND(User &user) {
	return ("412 " + user.get_username() + " :No text to send");
}

std::string ERR_NONICKNAMEGIVEN(std::string name) {
	return ("431 " + name + " :No nickname given");
}

std::string ERR_ERRONEUSNICKNAME(std::string name) {
	return ("432 " + name + " :Erroneus nickname");
}

std::string ERR_NICKNAMEINUSE(std::string name) {
	return ("433 " + name + " :Nickname is already in use");
}

std::string ERR_UNKNOWNCOMMAND(User &user, std::string command) {
	return ("421 " + user.get_username() + " " + command + " :Unknown command");
}

std::string ERR_NEEDMOREPARAMS(User &user, std::string command) {
	return ("461 " + user.get_username() + " " + command + " :Not enough parameters");
}

std::string ERR_ALREADYREGISTRED(User &user) {
	return ("462 " + user.get_username() + " :You may not reregister");
}

// Indicates the supplied channel name is not a valid.
// This is similar to, but stronger than, ERR_NOSUCHCHANNEL (403), which indicates 
// that the channel does not exist, but that it may be a valid name.
std::string ERR_BADCHANMASK(std::string channel_mask) {
	return ("476 " + channel_mask + ":Bad Channel Mask");
}


// Returned when a client tries to perform a channel-affecting command on a 
// channel which the client isn’t a part of.
std::string ERR_NOTONCHANNEL(User &user, Channel &chan) {
	return ("442 " + user.get_username() + " " + chan.get_ChannelName() + " :You're not on that channel");
}

// Returned when a client tries to invite <nick> to a channel they’re already joined to.
std::string ERR_USERONCHANNEL(User &user, std::string nick, Channel &chan) {
	return ("443 " + user.get_username() + " " + nick + " " + chan.get_ChannelName() + " :is already on channel");
}



// RPL_CHANNELMODEIS (324) 
//   "<client> <channel> <modestring> <mode arguments>..."
// Sent to a client to inform them of the currently-set modes of a channel. 
// <channel> is the name of the channel. <modestring> and <mode arguments> are a 
// mode string and the mode arguments (delimited as separate parameters) as 
// defined in the MODE message description.

std::string RPL_CHANNELMODEIS(User &user, Channel &chan) {
	return ("324 " + user.get_nickname() + " " + chan.get_ChannelName());
}
