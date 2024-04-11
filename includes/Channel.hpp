#pragma once

#include "IRC.hpp"

class Channel
{
	public:
		Channel();
		Channel(std::string name);
		Channel(Channel const &copy);
		Channel &operator=(Channel const &rhs);
		~Channel();
		std::string get_ChannelName() const;

	private:
		std::string _nameChannel;
		std::string _topic;
		std::string _pass;
		int 		_limitUser;
};
