#pragma once

#include <iostream>
#include <string>
#include <map>
#include "IRC.hpp"
#include "time.h"

class Channel
{
	public:
		Channel();
		Channel(std::string name);
		Channel(Channel const &copy);
		Channel &operator=(Channel const &rhs);
		~Channel();


// SETTERS - UNSETTERS
		void	set_ChannelUser(User &user);
		void	unset_ChannelUser(User& user);
		void	set_opChannel(std::string user);
		void	unset_opChannel(std::string user);
		void	set_limitUser(long nb);
		void	unset_limitUser(void);
		void	set_ChannelKey(std::string pass);
		void	unset_ChannelKey(void);
		void	set_inviteOnly(void);
		void	unset_inviteOnly(void);
		void	set_ChannelTopic(std::string topic);
		void	set_optopic(void);
		void	unset_optopic(void);

		void	add_inviteList(User &user);
		void	remove_inviteList(User& user);
		void	delete_inviteList(void);

// GETTERS
		std::string	get_ChannelName() const;
		std::string	get_ChannelTopic() const;
		time_t get_creationTime() const;
		std::string get_ChannelKey() const;
		long	get_limitUser() const;
		std::vector<User>	&get_ChannelUser();
		std::vector<std::string> &get_opUsers();
		std::vector<User>	&get_inviteList();


		bool	is_opChannel(std::string user);
		// bool	is_ValidKey(std::vector<std::string> key, int i);
		bool	is_ValidKey(std::string key);
		bool	is_UserInChannel(User &user);
		bool	is_inviteOnly(void);
		bool	is_opTopic(void);
		bool	is_InInviteList(User &user);


		void	sendMsg(User &user, std::string message, int code);
		void	creationTime(void);

	private:
		std::string _nameChannel;
		std::string _topic;
		std::string _pass;
		long 		_limitUser;
		bool		_inviteonly;
		bool		_optopic;
		time_t		_creationTime;
		std::vector<User> _chanUsers;
		std::vector<std::string> _opUsers;
		std::vector<User> _inviteList;
};
std::ostream &operator<<(std::ostream &o, Channel &src);

std::string print_Names(std::string nickname, Channel &channel);
