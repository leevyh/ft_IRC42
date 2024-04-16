#pragma once

#include <iostream>
#include <string>
#include <map>
#include "IRC.hpp"

class Channel
{
	public:
		Channel();
		Channel(std::string name);
		Channel(Channel const &copy);
		Channel &operator=(Channel const &rhs);
		~Channel();
		std::string	get_ChannelName() const;
		void		set_UserChannel(User &user);
		std::vector<User> & get_UserChannel();
		std::string get_ChannelTopic() const;
		void set_ChannelTopic(std::string topic);
		void set_opChannel(std::string user);
		bool is_opChannel(std::string user);


	private:
		std::string _nameChannel;
		std::string _topic;
		std::string _pass;
		int 		_limitUser;
		std::vector<User> _chanUsers;
		std::vector<std::string> _opUsers;
};