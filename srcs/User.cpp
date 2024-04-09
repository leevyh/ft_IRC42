#include "User.hpp"

User::User() : _fd() {
}

User::User(const int fd) : _fd(fd) {
	this->_nickname = "";
	this->_username = "";
	this->_realname = "";
	this->_status = false;
}

User::~User() {
//	close(this->_fd);
}

std::ostream &operator<<(std::ostream &o, const User &src) {
	o << "| " << src.get_fd() << "|";
	if (src.get_nickname().size()) {
		o << ' ' << src.get_nickname() << "|";
	}
	return (o);
}

User::User(const User &src) {
	*this = src;
}

User &User::operator=(const User &rhs) {
	if (this != &rhs) {
		this->_nickname = rhs._nickname;
		this->_username = rhs._username;
		this->_realname = rhs._realname;
		this->_status = false;
		this->_fd = rhs._fd;
	}
	return (*this);
}

/* ************************************************************************** */

void User::set_nickname(const std::string &nickname) {
	this->_nickname = nickname;
}

void User::set_username(const std::string &username) {
	this->_username = username;
}

void User::set_realname(const std::string &realname) {
	this->_realname = realname;
}

void User::set_password(const std::string &password) {
	this->_password = password;
}

void User::set_ip(const std::string &ip) {
	this->_ip = ip;
}

/* ************************************************************************** */

const std::string &User::get_nickname(void) const {
	return (this->_nickname);
}

const std::string &User::get_username(void) const {
	return (this->_username);
}

const std::string &User::get_realname(void) const {
	return (this->_realname);
}

const std::string &User::get_password(void) const {
	return (this->_password);
}

const std::string &User::get_ip(void) const {
	return (this->_ip);
}

int User::get_fd(void) const {
	return (this->_fd);
}

bool User::get_status(void) const {
	return (this->_status);
}

void User::joinBuffer(const char *buffer) {
	_buffer.append(buffer);
	return;
}

void User::receive(Server &server) {
	(void) server;
	if (_buffer.find("\n") == std::string::npos) {
		return;
	}
	size_t pos = _buffer.find("\r\n");
	if (pos == std::string::npos) {
		pos = _buffer.find("\n");
	}
	static int i = 0;
	while (pos != std::string::npos) {
		i++;
		// std::cout << "i: " << i << std::endl;
		std::string line = _buffer.substr(0, pos);
		// std::cout << "line: " << line << std::endl;
		if (line.size()) {
			std::cout << "receive: user fd:" << this->get_fd() << std::endl;
			parseClientMessage(server, line);
			line.clear();
//			return ;
		}
		_buffer.erase(0, _buffer.find("\n") + 1);
		pos = _buffer.find("\r\n");
		if (pos == std::string::npos) {
			pos = _buffer.find("\n");
		}
	}
}

std::vector<std::string> splitcmd(std::string line) {
	std::vector<std::string> cmd;
	char *arg = strtok((char *) line.c_str(),
					   "\r\n ");  // /!\ La string ne doit pas être constante avec utilisation de strtok
	while (arg != NULL && !line.empty()) {
		cmd.push_back(arg);
		arg = strtok(NULL, "\r\n ");
	}
	return (cmd);
}

void User::authentication(Server &server, Commands &cmd, std::vector<std::string> arg) {
	// if (!arg.empty() && arg.size() > 1) {
	// 	if (arg[0] == "CAP" && arg[1] == "LS") {
	// 		arg[0] = "CAP LS";
	// 		arg.erase(arg.begin() + 1);
	// 	}
	// }
	// for (size_t i = 0; i < arg.size(); i++)
	// 	std::cout << arg[i] << "#" << std::endl;
	if (!arg.empty()) {
		std::string cmds[] = {"PASS", "NICK", "USER", "CAP"};
		int i = 0;
		while (i < 4 && cmds[i].compare(arg[0]))
			i++;
		switch (i) {
			case 0:
				cmd.getcommand(server, *this, arg);
				break;
			case 1:
				cmd.getcommand(server, *this, arg);
				break;
			case 2:
				cmd.getcommand(server, *this, arg);
				break;
			case 3:
				cmd.getcommand(server, *this, arg);
				break;
			default:
				std::cout << "A CODER\n";
				break;
		}
	}
	std::cout << "Authentication: user fd:" << this->get_fd() << std::endl;
	if (!_nickname.empty() && !_username.empty() && !_realname.empty() && !_password.empty()) {
		std::cout << "_nickname: " << get_nickname() << " | _username: " << get_username() \
 << " | _realname: " << get_realname() << std::endl;
		_status = true;
		// WELCOME MESSAGE
		displayWelcome(server, *this);
	}
}

void User::parseClientMessage(Server &server, std::string line) {
	std::vector<std::string> splited_cmd = splitcmd(line);
	Commands cmd;
	std::cout << "parseClientMessage: user fd:" << this->get_fd() << std::endl;
	get_status() == true ? cmd.getcommand(server, *this, splited_cmd) : authentication(server, cmd, splited_cmd);
	line.clear();
	return;
}
