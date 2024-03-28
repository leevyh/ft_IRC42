#include "User.hpp"

User::User( void ) {
	this->_nick = "nick_test";
	this->_user = "user_test";
	this->_passwrd = "mdp";
}

User::User( const User& rhs ) {
	*this = rhs;
}

User::~User( void ) {}

User &User::operator=( const User& rhs ) {
	if (this != &rhs)
		*this = rhs;
	return (*this);
}

