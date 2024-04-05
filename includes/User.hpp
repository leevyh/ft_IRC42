#pragma once
#include <iostream>
#include <iostream>
#include <fstream>
#include <unistd.h>

class Server;
/* Each user is distinguished from other users by a unique nickname
having a maximum length of nine (9) characters.  See the protocol
grammar rules (section 2.3.1) for what may and may not be used in a
nickname.

While the maximum length is limited to nine characters, clients
SHOULD accept longer strings as they may become used in future
evolutions of the protocol. */
class User {
	private:
		User( void );

		std::string _nickname;
		std::string _username;
		std::string _realname;
		int _fd;

	public:
		User( const int fd );
		~User( void );

// SETTERS
		void set_nickname( const std::string& nickname );
		void set_username( const std::string& username );
		void set_realname( const std::string& realname );

// GETTERS
		const std::string&	get_nickname( void ) const;
		const std::string&	get_username( void ) const;
		const std::string&	get_realname( void ) const;
		int get_fd( void ) const;
};

std::ostream &operator<<( std::ostream &o, const User &src) ; 


// nick ::=  <any characters except NUL, CR, LF, chantype character, and SPACE> <possibly empty sequence of any characters except NUL, CR, LF, and SPACE>
// user ::=  <sequence of any characters except NUL, CR, LF, and SPACE>