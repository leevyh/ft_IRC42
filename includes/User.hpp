#pragma once
#include <iostream>


/* Each user is distinguished from other users by a unique nickname
having a maximum length of nine (9) characters.  See the protocol
grammar rules (section 2.3.1) for what may and may not be used in a
nickname.

While the maximum length is limited to nine characters, clients
SHOULD accept longer strings as they may become used in future
evolutions of the protocol. */
class User {
	private:
		std::string _nick;
		std::string _user;
		std::string _passwrd;

	public:
		User( void );
		User( const User& rhs );
		~User( void );

		User&	operator=( const User& rhs );
};


// nick ::=  <any characters except NUL, CR, LF, chantype character, and SPACE> <possibly empty sequence of any characters except NUL, CR, LF, and SPACE>
// user ::=  <sequence of any characters except NUL, CR, LF, and SPACE>
