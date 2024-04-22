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
			+ "!~" + user.get_username() + "@" + user.get_ip() + ".ip");
}

std::string RPL_YOURHOST(Server &server, User &user) {
	return ("002 " + user.get_nickname() + " :Your host is "
			+ server.get_networkname() + ", Network, running v0.1");
}

std::string RPL_CREATED(User &user) {
	return ("003 " + user.get_nickname() + " :This server was created Sun 20 Apr 2042 during the total eclipse");
}

std::string RPL_MYINFO(Server &server, User &user) {
	return ("004 " + user.get_nickname() + " " + server.get_networkname() + \
	" v0.1 <available user modes> <available channel modes>");
}

/* ************************************************************************** */

void	displayInfosChannel(Server &server, User &user, Channel &channel) {
//	server.sendMsg(user, RPL_JOIN(user, channel), 1);
	server.sendMsg(user, RPL_NAMES(user, channel), 1);
	server.sendMsg(user, RPL_ENDOFNAMES(user, channel), 1);
	server.sendMsg(user, RPL_CREATIONTIME(user, channel), 1);
}

std::string RPL_CREATIONTIME(User &user, Channel &channel) {
	time_t	creationTime = channel.get_creationTime();
	std::stringstream ss;
	ss << creationTime;
	return ("329 " + user.get_username() + " " + channel.get_ChannelName() + " " + ss.str());
}

std::string RPL_NOTOPIC(User &user, Channel &channel) {
	return ("331 " + user.get_username() + " " + channel.get_ChannelName() + " :" + channel.get_ChannelTopic());
}

std::string RPL_TOPIC(User &user, Channel &channel) {
	return ("332 " + user.get_username() + " " + channel.get_ChannelName() + " :" + channel.get_ChannelTopic());
}

// Sent to a client as a reply to the INVITE command when used with no parameter, to indicate a channel the client was invited to.
std::string RPL_INVITELIST(User &user, Channel &channel) {
	return ("336 " + user.get_username() + " " + channel.get_ChannelName());
}

// Sent as a reply to the INVITE command when used with no parameter, this numeric indicates the end of invitations a client received.
std::string RPL_ENDOFINVITELIST(User &user) {
	return ("337 " + user.get_username() + " :End of /INVITE list");
}

std::string RPL_NAMES(User &user, Channel &channel) {
	std::string names = "";
	std::vector<User> users = channel.get_UserChannel();
	for (std::vector<User>::iterator it = users.begin(); it != users.end(); ++it) {
		names += print_Names(it->get_nickname(), channel) + it->get_nickname() + " ";
	}
	return ("353 " + user.get_username() + " = " + channel.get_ChannelName() + " :" + names);
}

std::string RPL_ENDOFNAMES(User &user, Channel &channel) {
	return ("366 " + user.get_username() + " " + channel.get_ChannelName() + " :End of /NAMES list");
}

/* ************************************************************************** */

std::string ERR_NOSUCHNICK(User &user, std::string nickname) {
	return ("401 " + user.get_username() + " " + nickname + " :No suck nick/channel");
}

std::string ERR_NOSUCHSERVER(User &user, std::string server_name) {
	return ("402 " + user.get_username() + " " + server_name + " :No such server");
}

std::string ERR_NOSUCHCHANNEL(User &user, std::string channel) {
	return ("403 " + user.get_nickname() + " " + channel + " :No such channel");
}

std::string ERR_CANNOTSENDTOCHAN(User &user, std::string channel) {
	return ("404 " + user.get_username() + " " + channel + " :Cannot send to channel");
}

std::string ERR_NOORIGIN(User &user) {
	return ("409 " + user.get_username() + ":No origin specified");
}

std::string ERR_NORECIPIENT(User &user, std::string command) {
	return ("411 " + user.get_username() + " :No recipient given (" + command + ")");
}

std::string ERR_NOTEXTTOSEND(User &user) {
	return ("412 " + user.get_username() + " :No text to send");
}

std::string ERR_UNKNOWNCOMMAND(User &user, std::string command) {
	return ("421 " + user.get_username() + " " + command + " :Unknown command");
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

std::string ERR_USERNOTINCHANNEL(User &user, std::string nick, Channel& chan) {
	return ("441 " + user.get_username() + " " + nick + " " + chan.get_ChannelName() + " :They aren't on that channel");
}

std::string ERR_NOTONCHANNEL(User &user, Channel &chan) {
	return ("442 " + user.get_username() + " " + chan.get_ChannelName() + " :You're not on that channel");
}

std::string ERR_USERONCHANNEL(User &user, std::string nick, Channel &chan) {
	return ("443 " + user.get_username() + " " + nick + " " + chan.get_ChannelName() + " :is already on channel");
}

std::string ERR_NEEDMOREPARAMS(User &user, std::string command) {
	return ("461 " + user.get_username() + " " + command + " :Not enough parameters");
}

std::string ERR_ALREADYREGISTRED(User &user) {
	return ("462 " + user.get_username() + " :You may not reregister");
}

std::string ERR_UNKNOWNMODE(User &user, std::string modechar) {
	return ("472 " + user.get_username() + " " + modechar + " :is unknown mode char to me");
}

std::string ERR_INVITEONLYCHAN(User &user, Channel &channel) {
	return ("473 " + user.get_username() + " " + channel.get_ChannelName() + " :Cannot join channel (+i)");
}

std::string ERR_BADCHANNELKEY(User &user, Channel &channel) {
	return ("475 " + user.get_username() + " " + channel.get_ChannelName() + " :Cannot join channel (+k)");
}

std::string ERR_BADCHANMASK(std::string channel_mask) {
	return ("476 " + channel_mask + " :Bad Channel Mask");
}

std::string ERR_CHANOPRIVSNEEDED(User &user, Channel &chan) {
	return ("482 " + user.get_username() + " " + chan.get_ChannelName() + " :You're not channel operator");
}

// RPL_CHANNELMODEIS (324) 
//   "<client> <channel> <modestring> <mode arguments>..."
// Sent to a client to inform them of the currently-set modes of a channel. 
// <channel> is the name of the channel. <modestring> and <mode arguments> are a 
// mode string and the mode arguments (delimited as separate parameters) as 
// defined in the MODE message description.

// std::string RPL_CHANNELMODEIS(User &user, Channel &chan) {
// 	return ("324 " + user.get_username() + " " + chan.get_ChannelName() + );
// }

// Parameters: <channel> *( ( "-" / "+" ) *<modes> *<modeparams>
// If <modeparams> is not given, the RPL_CHANNELMODEIS (324) numeric is returned.

/* ************************************************************************** */

std::string RPL_MODE(User &user, Channel &channel, std::string modestring, std::string mode_arg) {
	std::string msg = user.get_nickname() + "!~" + user.get_username() + "@" + user.get_ip() + ".ip";
	if (mode_arg.empty())
		return (msg + " MODE " + channel.get_ChannelName() + " " + modestring);
	return (msg + " MODE " + channel.get_ChannelName() + " " + modestring + " " + mode_arg);
}

std::string RPL_JOIN(User &user, Channel &channel) {
	std::string msg = user.get_nickname() + "!~" + user.get_username() + "@" + user.get_ip() + ".ip";
	return (msg + " JOIN :" + channel.get_ChannelName());
}

std::string RPL_EDITTOPIC(User &user, Channel &channel, std::string topic) {
	std::string msg = user.get_nickname() + "!~" + user.get_username() + "@" + user.get_ip() + ".ip";
	return (msg + " TOPIC " + channel.get_ChannelName() + " :" + topic);
}

std::string RPL_PRIVMSG(User &user, std::string recipient, std::string message) {
	std::string rpl = user.get_nickname() + "!~" + user.get_username() + "@" + user.get_ip() + ".ip";
	return (rpl + " PRIVMSG " + recipient + " :" + message);
}

std::string RPL_PART(User &user, Channel &channel, std::string part_message, int code)
{
	std::string msg = user.get_nickname() + "!~" + user.get_username() + "@" + user.get_ip() + ".ip";
	if (code == 1)
		return (msg + " PART " + channel.get_ChannelName());
	else
		return (msg + " PART " + channel.get_ChannelName() + " :" + part_message);
}


