#include "Channel.hpp"

Channel::Channel() {
	_nameChannel = "";
	_pass = "";
	_limitUser = -1;
	_inviteonly = false;
}

Channel::Channel(std::string name) : _nameChannel(name) {
	_pass = "";
	_limitUser = -1;
	_inviteonly = false;
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
		_inviteonly = rhs._inviteonly;
	}
	return (*this);
}

Channel::~Channel() {}

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

long	Channel::get_limitUser() const {return (_limitUser);}

std::string	Channel::get_password() const {return (_pass);}

std::string Channel::get_ChannelKey() const {return (_pass);}

/* ************************************************************************** */

void	Channel::set_ChannelUser(User &user) {_chanUsers.push_back(user);}

void	Channel::unset_ChannelUser(User& user){
	for (std::vector<User>::iterator it = _chanUsers.begin(); it != _chanUsers.end(); ++it) {
		if (it->get_username() == user.get_username()) {
			_chanUsers.erase(it);
			return;
		}
	}
	return;
}

void	Channel::set_ChannelTopic(std::string topic) {_topic = topic;}

void	Channel::set_opChannel(std::string user) {_opUsers.push_back(user);}

void	Channel::unset_opChannel(std::string user) {
	for (std::vector<std::string>::iterator it = _opUsers.begin(); it != _opUsers.end(); ++it)
		if (*it == user) {
			_opUsers.erase(it);
			return;
		}
}

void	Channel::set_limitUser(long nb) {
	_limitUser = nb;
	std::cout << "_limitUser: " << _limitUser << std::endl;}

void	Channel::unset_limitUser(void) {_limitUser = -1;}

void	Channel::set_password(std::string pass) {_pass = pass;}

void	Channel::unset_password(void) {_pass = "";}

void	Channel::set_inviteOnly(void) {_inviteonly = true;}

void	Channel::unset_inviteOnly(void) {_inviteonly = false;}

/* ************************************************************************** */

bool	Channel::is_opChannel(std::string user) {
	for (std::vector<std::string>::iterator it = _opUsers.begin(); it != _opUsers.end(); ++it) {
		if (*it == user)
			return (true);
	}
	return (false);
}

bool	Channel::is_ValidKey(std::vector<std::string> key, int i) {
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
	for (std::vector<User>::iterator it = _chanUsers.begin(); it != _chanUsers.end(); ++it) {
		if (it->get_nickname() == user.get_nickname())
			return (true);
	}
	return (false);
}

bool	Channel::is_inviteOnly(void) {
	if (_inviteonly == true)
		return (true);
	return (false);
}


void	Channel::sendMsg(std::string message) {
	std::string msg;

	msg = ":" + message + "\r\n";
	for (std::vector<User>::iterator it = this->get_UserChannel().begin(); \
		it != this->get_UserChannel().end(); ++it)
		if (send(it->get_fd(), msg.c_str(), msg.length(), 0) == -1)
			std::perror("send:");
}
