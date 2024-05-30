#include "Channel.hpp"

Channel::Channel() {
	_name = "";
	_key = "";
	_limitUser = -1;
	_inviteOnly = false;
	_opTopic = false;
}

Channel::Channel(std::string name) : _name(name) {
	_key = "";
	_limitUser = -1;
	_inviteOnly = false;
	_opTopic = false;
}

Channel::Channel(Channel const &copy) {*this = copy;}

Channel &Channel::operator=(Channel const &rhs) {
	if (this != &rhs) {
		_name = rhs._name;
		_chanUsers = rhs._chanUsers;
		_opUsers = rhs._opUsers;
		_opTopic = rhs._opTopic;
		_topic = rhs._topic;
		_creationTime = rhs._creationTime;
		_key = rhs._key;
		_limitUser = rhs._limitUser;
		_inviteOnly = rhs._inviteOnly;
		_inviteList = rhs._inviteList;
	}
	return (*this);
}

Channel::~Channel() {}

std::ostream &operator<<(std::ostream &o, Channel &src) {
	o << "Name: " << src.get_ChannelName() << " | Users(s) : ";
	for (std::vector<User>::iterator it = src.get_ChannelUser().begin(); 
		it != src.get_ChannelUser().end(); ++it) {
		std::cout << it->get_username() << " - OP : ";
		if (src.is_opChannel(it->get_username()))
			std::cout << "Y" << std::endl;
		else
			std::cout << "N" << std::endl;
	}
	return (o);
}

/* ************************************************************************** */

void	Channel::set_ChannelUser(User &user) {_chanUsers.push_back(user);}

void	Channel::unset_ChannelUser(User& user) {
	for (std::vector<User>::iterator it = _chanUsers.begin(); it != _chanUsers.end(); ++it) {
		if (it->get_nickname() == user.get_nickname()) {
			_chanUsers.erase(it);
			return;
		}
	}
	return;
}

void	Channel::set_opChannel(std::string user) {_opUsers.push_back(user);}

void	Channel::unset_opChannel(std::string user) {
	for (std::vector<std::string>::iterator it = _opUsers.begin(); it != _opUsers.end(); ++it)
		if (*it == user) {
			_opUsers.erase(it);
			return;
		}
}

void	Channel::set_limitUser(long nb) {_limitUser = nb;}

void	Channel::unset_limitUser(void) {_limitUser = -1;}

void	Channel::set_ChannelKey(std::string pass) {_key = pass;}

void	Channel::unset_ChannelKey(void) {_key = "";}

void	Channel::set_inviteOnly(void) {_inviteOnly = true;}

void	Channel::unset_inviteOnly(void) {_inviteOnly = false;}

void	Channel::set_ChannelTopic(std::string topic) {_topic = topic;}

void	Channel::set_opTopic(void) {_opTopic = true;}

void	Channel::unset_opTopic(void) {_opTopic = false;}

void	Channel::add_inviteList(User &user) {_inviteList.push_back(user);}

void	Channel::remove_inviteList(User& user) {
	for (std::vector<User>::iterator it = _inviteList.begin(); it != _inviteList.end(); ++it) {
		if (it->get_nickname() == user.get_nickname()) {
			_inviteList.erase(it);
			return;
		}
	}
	return;
}

void	Channel::delete_inviteList(void) {_inviteList.clear();}

/* ************************************************************************** */

std::string	Channel::get_ChannelName() const {return (_name);}

std::vector<User>	&Channel::get_ChannelUser() {return (_chanUsers);}

std::vector<std::string> &Channel::get_opUsers() {return (_opUsers);}

std::string	Channel::get_ChannelTopic() const {return (_topic);}

time_t	Channel::get_creationTime() const {return (_creationTime);}

std::string Channel::get_ChannelKey() const {return (_key);}

long	Channel::get_limitUser() const {return (_limitUser);}

std::vector<User>	&Channel::get_inviteList() {return (_inviteList);}

/* ************************************************************************** */

bool	Channel::is_opChannel(std::string user) {
	for (std::vector<std::string>::iterator it = _opUsers.begin(); it != _opUsers.end(); ++it) {
		if (*it == user)
			return (true);
	}
	return (false);
}

bool	Channel::is_ValidKey(std::string key) {
	if (key.empty())
		return (false);
	if (key == _key)
		return (true);
	return (false);
}

bool	Channel::is_UserInChannel(User &user) {
	for (std::vector<User>::iterator it = _chanUsers.begin(); it != _chanUsers.end(); ++it) {
		if (it->get_nickname() == user.get_nickname())
			return (true);
	}
	return (false);
}

bool	Channel::is_inviteOnly(void) {
	if (_inviteOnly == true)
		return (true);
	return (false);
}

bool	Channel::is_opTopic(void) {
	if (_opTopic == true)
		return (true);
	return (false);
}

bool	Channel::is_InInviteList(User &user) {
	for (std::vector<User>::iterator it = _inviteList.begin(); it != _inviteList.end(); ++it) {
		if (it->get_nickname() == user.get_nickname())
			return (true);
	}
	return (false);
}

/* ************************************************************************** */

void	Channel::sendMsg(User &user, std::string message, int code) {
	std::string msg;

	msg = ":" + message + "\r\n";
	switch (code) {
		case 1: // A TOUS LES USERS DU CHANNEL
			for (std::vector<User>::iterator it = this->get_ChannelUser().begin(); \
				it != this->get_ChannelUser().end(); ++it)
				if (send(it->get_fd(), msg.c_str(), msg.length(), 0) == -1)
					std::perror("send:");
		case 2: // A TOUS SAUF LE USER
			for (std::vector<User>::iterator it = this->get_ChannelUser().begin(); \
				it != this->get_ChannelUser().end(); ++it)
				if (it->get_fd() != user.get_fd())
					if (send(it->get_fd(), msg.c_str(), msg.length(), 0) == -1)
						std::perror("send:");
	}
}

void	Channel::creationTime(void) {
	time_t now = time(NULL);
	_creationTime = now;
}
