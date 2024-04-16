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
	}
	return (*this);
}

void	Channel::set_UserChannel(User &user) {
	_chanUsers.push_back(user);
}

std::vector<User> &Channel::get_UserChannel() {
	return (_chanUsers);
}

std::string Channel::get_ChannelName() const {
	return (_nameChannel);
}

Channel::~Channel() {
}