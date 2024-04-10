#include "IRC.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "Commands.hpp"

void displayWelcome(Server &server, User &user) {
	if (send(user.get_fd(), RPL_WELCOME(server, user).c_str(), RPL_WELCOME(server, user).size(), 0) == -1 ||
		send(user.get_fd(), RPL_YOURHOST(server, user).c_str(), RPL_YOURHOST(server, user).length(), 0) == -1 ||
		send(user.get_fd(), RPL_CREATED(user).c_str(), RPL_CREATED(user).length(), 0) == -1 ||
		send(user.get_fd(), RPL_MYINFO(server, user).c_str(), RPL_MYINFO(server, user).length(), 0) == -1) {
		std::perror("send:");
	}
}

// "<client> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]"
std::string RPL_WELCOME(Server &server, User &user) {
	return ("001 " + user.get_nickname() + " :Welcome to the "
			+ server.get_networkname() + " Network, " + user.get_nickname()
			+ "!~" + user.get_username() + "@" + user.get_ip() + "\r\n");
}

// "<client> :Your host is <servername>, running version <version>"
std::string RPL_YOURHOST(Server &server, User &user) {
	return ("002 " + user.get_nickname() + " :Your host is "
			+ server.get_networkname() + ", Network, running v0.1\r\n");
}

// "<client> :This server was created <datetime>"
std::string RPL_CREATED(User &user) {
	return ("003 " + user.get_nickname() + " :This server was created Sun 20 Apr 2042 during the total eclipse\r\n");
}

// "<client> <servername> <version> <available user modes>
// <available channel modes> [<channel modes with a parameter>]"
std::string RPL_MYINFO(Server &server, User &user) {
	return ("004 " + user.get_nickname() + " " + server.get_networkname() + \
	" v0.1 <available user modes> <available channel modes>\r\n");
}

/* ************************************************************************** */

std::string ERR_NONICKNAMEGIVEN(std::string name) {
	return ("431 " + name + " :No nickname given\r\n");
}

std::string ERR_ERRONEUSNICKNAME(std::string name) {
	return ("432 " + name + " :Erroneus nickname\r\n");
}

std::string ERR_NICKNAMEINUSE(std::string name) {
	return ("433 " + name + " :Nickname is already in use\r\n");
}

std::string ERR_UNKNOWNCOMMAND(User &user, std::string command) {
	return ("421 " + user.get_username() + " " + command + " :Unknown command\r\n");
}

std::string ERR_NEEDMOREPARAMS(User &user, std::string command) {
	return ("461 " + user.get_username() + " " + command + " :Not enough parameters\r\n");
}

std::string ERR_ALREADYREGISTRED(User &user) {
	return ("462 " + user.get_username() + " :You may not reregister\r\n");
}

/* ************************************************************************** */

//   "<client> <nickname> :No such nick/channel"
std::string ERR_NOSUCHNICK(User &user, std::string nickname) // (401)
{
	return ("401 " + user.get_username() + " " + nickname + " :No suck nick/channel\r\n");
}

//   "<client> <server name> :No such server"
std::string ERR_NOSUCHSERVER(User &user, std::string server_name) // (402)
{
	return ("402 " + user.get_username() + " " + server_name + " :No such server\r\n");
}



// ERR_NOTOPLEVEL (413); 
// ERR_WILDTOPLEVEL (414);


/*

ERR_NOSUCHCHANNEL (403) 
  "<client> <channel> :No such channel"
ERR_NORECIPIENT (411) 
  "<client> :No recipient given (<command>)"
Returned by the PRIVMSG command to indicate the message wasn’t delivered because there was no recipient given.
ERR_NOTEXTTOSEND (412) 
  "<client> :No text to send"
Returned by the PRIVMSG command to indicate the message wasn’t delivered because there was no text to send.
RPL_AWAY (301) 
  "<client> <nick> :<message>"
Indicates that the user with the nickname <nick> is currently away and sends the away message that they set.
*/