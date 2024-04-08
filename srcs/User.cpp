#include "User.hpp"

User::User() : _fd()
{}

User::User( const int fd ) : _fd(fd)
{
    this->_nickname = "";
    this->_username = "";
    this->_realname = "";
    this->_status = true;
}

User::~User() {
//	close(this->_fd);
}

std::ostream &operator<<(std::ostream &o, const User &src)
{
	o << "| " << src.get_fd() << "|";
	if (src.get_nickname().size())
		o << ' ' << src.get_nickname() << "|";
	return (o);
}

User::User( const User & src ) {
	*this = src;
}

User & User::operator=( const User & rhs ) {
	if (this != &rhs) {
		this->_nickname = rhs._nickname;
		this->_username = rhs._username;
		this->_realname = rhs._realname;
		this->_status = true;					// A verifier
		this->_fd = rhs._fd;
	}
	return (*this);
}

void User::set_nickname(const std::string &nickname) {
	this->_nickname = nickname;
}

void User::set_username( const std::string &username ) {
	this->_username = username;
}

void User::set_realname( const std::string &realname ) {
	this->_realname = realname;
}

int User::get_fd( void ) const {
	return (this->_fd); 
}

const std::string&	User::get_nickname( void ) const {
	return (this->_nickname);
}

const std::string&	User::get_username( void ) const {
	return (this->_username);
}

const std::string&	User::get_realname( void ) const {
	return (this->_realname);
}

bool User::get_status( void ) const {
	return (this->_status);
}

void User::joinBuffer( const char* buffer ){
	_buffer += buffer;
	return;
}

void User::receive( Server& server ){
	if (_buffer.find("\n") == std::string::npos)
		return;
	size_t pos = _buffer.find("\r\n");
	if (pos == std::string::npos)
		pos = _buffer.find("\n");
	while (pos != std::string::npos)
	{
		std::string	line = _buffer.substr(0, pos);
		if (line.size())
			// parseClientMessage(server, line);
		_buffer.erase(0, _buffer.find("\n") + 1);
		pos = _buffer.find("\r\n");
		if (pos == std::string::npos)
			pos = _buffer.find("\n");
	}
}
