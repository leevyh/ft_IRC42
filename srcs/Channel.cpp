#include "Channel.hpp"

Channel::Channel() {
	_nameChannel = "";
	_topic = "";
	_pass = "";
}

Channel::Channel(std::string name) : _nameChannel(name) {
	_topic = "";
	_pass = "";
}

Channel::Channel(Channel const &copy) {
	*this = copy;
}

Channel &Channel::operator=(Channel const &rhs) {
	if (this != &rhs) {
		_nameChannel = rhs._nameChannel;
		_topic = rhs._topic;
		_pass = rhs._pass;
	}
	return (*this);
}

std::string Channel::get_ChannelName() const {
	return (_nameChannel);
}



Channel::~Channel() {
}
