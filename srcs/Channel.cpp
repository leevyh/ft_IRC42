#include "Channel.hpp"

Channel::Channel() {
	_nameChannel = "";
	_pass = "";
	_limitUser = -1;
}

Channel::Channel(std::string name) : _nameChannel(name) {
	_pass = "";
	_limitUser = -1;
}

Channel::Channel(Channel const &copy) {*this = copy;}

Channel &Channel::operator=(Channel const &rhs) {
	if (this != &rhs) {
		_nameChannel = rhs._nameChannel;
		_topic = rhs._topic;
		_pass = rhs._pass;
		_chanUsers = rhs._chanUsers;
		_opUsers = rhs._opUsers;
		_limitUser = rhs._limitUser;
	}
	return (*this);
}

Channel::~Channel() {}

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

/* ************************************************************************** */

std::string	Channel::get_ChannelName() const {return (_nameChannel);}

std::vector<User>	&Channel::get_UserChannel() {return (_chanUsers);}

std::string	Channel::get_ChannelTopic() const {return (_topic);}

int	Channel::get_limitUser() const {return (_limitUser);}

std::string	Channel::get_password() const {return (_pass);}

/* ************************************************************************** */

void	Channel::set_UserChannel(User &user) {_chanUsers.push_back(user);}

void	Channel::set_ChannelTopic(std::string topic) {_topic = topic;}

void	Channel::set_opChannel(std::string user) {_opUsers.push_back(user);}

void	Channel::unset_opChannel(std::string user) {
	for (std::vector<std::string>::iterator it = _opUsers.begin(); it != _opUsers.end(); ++it)
		if (*it == user) {
			_opUsers.erase(it);
			return;
		}
}

void	Channel::set_limitUser(int nb) {_limitUser = nb;}

void	Channel::unset_limitUser(void) {_limitUser = -1;}

void	Channel::set_password(std::string pass) {_pass = pass;}

void	Channel::unset_password(void) {_pass = "";}

/* ************************************************************************** */

bool	Channel::is_opChannel(std::string user) {
	for (std::vector<std::string>::iterator it = _opUsers.begin(); it != _opUsers.end(); ++it) {
		if (*it == user)
			return (true);
	}
	return (false);
}
