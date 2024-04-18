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
		std::vector<User>	&get_UserChannel();
		std::string	get_ChannelTopic() const;
		long	get_limitUser() const;
		std::string	get_password() const;
		std::string get_ChannelKey() const;
		void	set_ChannelUser(User &user);
		void	unset_ChannelUser(User& user);
		void	set_ChannelTopic(std::string topic);
		void	set_opChannel(std::string user);
		void	unset_opChannel(std::string user);
		void	set_limitUser(long nb);
		void	unset_limitUser(void);
		void	set_password(std::string pass);
		void	unset_password(void);
		void	set_inviteOnly(void);
		void	unset_inviteOnly(void);

		bool	is_opChannel(std::string user);
		bool	is_ValidKey(std::vector<std::string> key, int i);
		bool	is_UserInChannel(User &user);
		bool	is_inviteOnly(void);

		void	sendMsg(std::string message);

	private:
		std::string _nameChannel;
		std::string _topic;
		std::string _pass;
		long 		_limitUser;
		bool		_inviteonly;
		// bool		_optopic;
		std::vector<User> _chanUsers;
		std::vector<std::string> _opUsers;
};
std::ostream &operator<<(std::ostream &o, Channel &src);
