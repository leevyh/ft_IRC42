#include "User.hpp"

User::User() : _fd() {}

User::User(const int fd) : _fd(fd) {
	this->_nickname = "";
	this->_username = "";
	this->_realname = "";
	this->_status = true;
	this->_authenticated = false;
}

User::~User() {}

User::User(const User &src) {
	*this = src;
}

User &User::operator=(const User &rhs) {
	if (this != &rhs) {
		this->_nickname = rhs._nickname;
		this->_username = rhs._username;
		this->_realname = rhs._realname;
		this->_password = rhs._password;
		this->_ip = rhs._ip;
		this->_port = rhs._port;
		this->_status = true; // ? rhs._status ?
		this->_authenticated = false; // ? rhs._authenticated ?
		this->_fd = rhs._fd;
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &o, const User &src) {
	o << "| " << src.get_fd() << "|";
	if (src.get_nickname().size()) {
		o << ' ' << src.get_nickname() << "|";
	}
	return (o);
}

/* ************************************************************************** */

void User::set_nickname(const std::string &nickname) {
	this->_nickname = nickname;
}

void User::set_username(const std::string &username) {
	this->_username = username;
}

void	User::set_status(bool status) {
	this->_status = status;
}

void	User::set_authenticated(bool status) {
	this->_authenticated = status;
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

void User::set_port(unsigned short &port) {
	this->_port = port;
}

void User::set_lastping(const int &ping) {
	this->_lastping = ping;
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

unsigned short &User::get_port(void) {
	return (this->_port);
}

int User::get_fd(void) const {
	return (this->_fd);
}

bool User::get_status(void) const {
	return (this->_status);
}

bool User::get_authenticated(void) const {
	return (this->_authenticated);
}

/* ************************************************************************** */

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
		std::string line = _buffer.substr(0, pos);
		if (line.size()) {
			std::cout << "Line : " << line << std::endl;
			parseClientMessage(server, line);
			line.clear();
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
	char *arg = strtok((char *) line.c_str(), "\r\n ");
	while (arg != NULL && !line.empty()) {
		cmd.push_back(arg);
		arg = strtok(NULL, "\r\n ");
	}
	return (cmd);
}

void User::authentication(Server &server, Commands &cmd, std::vector<std::string> arg) {
	if (!arg.empty()) {
		std::string cmds[] = {"PASS", "NICK", "USER", "CAP"};
		int i = 0;
		while (i < 4 && cmds[i].compare(arg[0]))
			i++;
		switch (i) {
			case 0: case 1: case 2: case 3:
				cmd.getcommand(server, *this, arg);
				break;
			default:
				server.sendMsg(*this, "You are not connected to the server.", 1);
				if (!(this->get_password().empty()))
					server.sendMsg(*this, "Password: OK", 1);
				else {
					server.sendMsg(*this, "You need to use /PASS to connect first.", 1);
					break;
				}
				!(this->get_nickname().empty()) ? server.sendMsg(*this, "Nickname: " + this->get_nickname(), 1) \
				: server.sendMsg(*this, "You need to use /NICK to set your nickname first.", 1);
				!(this->get_username().empty()) ? server.sendMsg(*this, "Username: " + this->get_username(), 1) \
				: server.sendMsg(*this, "You need to use /USER to set your username first.", 1);
		}
	}
	if (!_nickname.empty() && !_username.empty() && !_realname.empty() && !_password.empty()) {
		_authenticated = true;
		displayWelcome(server, *this);
	}
}

void User::parseClientMessage(Server &server, std::string line) {
	std::vector<std::string> splited_cmd = splitcmd(line);
	Commands cmd;
	if (get_authenticated() == true)
		cmd.getcommand(server, *this, splited_cmd);
	else
		authentication(server, cmd, splited_cmd);
	line.clear();
	return;
}
