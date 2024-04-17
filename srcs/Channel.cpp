#include "Channel.hpp"

Channel::Channel() {
	_nameChannel = "";
	_pass = "";
}

Channel::Channel(std::string name) : _nameChannel(name) {
	_pass = "";
	_limitUser = 0;
}

Channel::Channel(Channel const &copy) {
	*this = copy;
}

Channel &Channel::operator=(Channel const &rhs) {
	if (this != &rhs) {
		_nameChannel = rhs._nameChannel;
		_topic = rhs._topic;
		_pass = rhs._pass;
		_chanUsers = rhs._chanUsers;
		_opUsers = rhs._opUsers;
	}
	return (*this);
}

void	Channel::set_UserChannel(User &user) {
	_chanUsers.push_back(user);
}

std::vector<User> &Channel::get_UserChannel() {
	return (_chanUsers);
}

std::string Channel::get_ChannelKey() const {
	return (_pass);
}

std::string Channel::get_ChannelName() const {
	return (_nameChannel);
}

std::string Channel::get_ChannelTopic() const {
	return (_topic);
}

void Channel::set_ChannelTopic(std::string topic) {
	_topic = topic;
}

void	Channel::set_opChannel(std::string user) {
	std::cout << "Adding " << user << " to op list" << std::endl;
	_opUsers.push_back(user);
}

bool Channel::is_opChannel(std::string user) { //TODO FIX THIS
	for (std::vector<std::string>::iterator it = _opUsers.begin(); it != _opUsers.end(); ++it) {
		if (*it == user)
			return (true);
	}
	return (false);
}

Channel::~Channel() {
}

/* ************************************************************************** */

std::ostream &operator<<(std::ostream &o, Channel &src) {
	o << "Name: " << src.get_ChannelName() << " | Users(s) : ";
	for (std::vector<User>::iterator it = src.get_UserChannel().begin(); 
		it != src.get_UserChannel().end(); ++it) {
		std::cout << it->get_username() << " - OP : ";
		if (src.is_opChannel(it->get_username()))
			std::cout << "Y" << std::endl;
		else
			std::cout << "N" << std::endl;
	}
	return (o);
}

bool	Channel::is_ValidKey(std::vector<std::string> key, int i)
{
//	if (key.empty() || key.size() < (unsigned long)i)
//		return (false);
//	if (key[i] == _pass)
//		return (true);
//	return (false);

	(void)key;
	(void)i;
	return (true);
}

bool	Channel::is_UserInChannel(User &user) {
	for (std::vector<User>::iterator it = _chanUsers.begin(); it != _chanUsers.end(); ++it)
	{
		if (it->get_nickname() == user.get_nickname())
			return (true);
	}
	return (false);
}

void	Channel::unsetChannelUser(User& user){

	for (std::vector<User>::iterator it = _chanUsers.begin(); it != _chanUsers.end(); ++it)
	{
		if (it->get_username() == user.get_username())
		{
			_chanUsers.erase(it);
			return;
		}
	}
	return;
}



