#include "Server.hpp"
#include "IRC.hpp"
#include "User.hpp"

Server::Server()
{
    _port = 6667;
    _password = "default";
    _nb_of_users = 0;
}

Server::Server(long port, std::string password) : _port(port), _password(password)
{
    std::cout << "Server created with port: " << _port << " and password: " << _password << std::endl;
    _nb_of_users = 0;
}

Server::Server(Server const &copy)
{
    *this = copy;
}

Server &Server::operator=(Server const &rhs)
{
    if (this != &rhs)
    {
        _port = rhs._port;
        _password = rhs._password;
    }
    return (*this);
}

long Server::get_Port(void) const { return (_port); }
std::string Server::get_Password(void) const { return (_password); }

void Server::init_serv(void)
{
    int server_socket;
    // int yes = 1;
    // char ip_addr[INET_ADDRSTRLEN];

    // char	msg[1024];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        // throw exception("Error creating socket")
        exit(1);
    }
    // if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    // {
    // 	std::cerr << "Error Setsockopt" << std::endl;
    // }

    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Error setting socket to non-blocking" << std::endl;
        // throw exception("Error setting socket to non-blocking")
        exit(1);
    }

    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(_port);
    _server_addr.sin_addr.s_addr = INADDR_ANY;

    // inet_ntop(AF_INET, &(_server_addr.sin_addr), ip_addr, INET_ADDRSTRLEN);

    if (bind(server_socket, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) == -1)
    {
        std::cerr << "Error binding socket" << std::endl;
        // throw exception("Error binding socket")
        exit(1);
    }

    if (listen(server_socket, 200) == -1)
    {
        std::cerr << "Error listening on socket" << std::endl;
        // throw exception("Error listening on socket")
        exit(1);
    }

    // std::cout << "End of init_serv" << std::endl;
    // std::cout << "Server listening on port " << _port << std::endl;
    // std::cout << "Socket: " << server_socket << std::endl;
    // std::cout << "Server address: " << inet_ntoa(_server_addr.sin_addr) << std::endl;
    // std::cout << "Server port: " << ntohs(_server_addr.sin_port) << std::endl;
    pollfdmap.push_back(pollfd());
    pollfdmap.back().fd = server_socket;
    pollfdmap.back().events = POLLIN;
}

void Server::start_serv(void)
{
    if (poll(&pollfdmap[0], pollfdmap.size(), 10000) == -1) // -1 means wait indefinitely but we can change it to a timeout 1000000 for 1 second
    {
        if (signal_value == false)
        {
            std::cerr << "Signal" << std::endl;
            // throw exception("poll exception")
            exit(1);
        }
    }
    if (pollfdmap[0].revents == POLLIN)
        new_Connection_Client();
    else
    {
        sleep(1);
        // std::cout << "No new connection" << std::endl;
    }
}

void Server::new_Connection_Client(void)
{
    struct sockaddr_in user_addr;
    socklen_t user_addr_len = sizeof(user_addr);

    std::cout << "Socket server juste avant le fcntl :" << pollfdmap[0].fd << std::endl;

    int client_socket = accept(pollfdmap[0].fd, (struct sockaddr *)&user_addr, &user_addr_len);
    std::cout << "Socket server juste apres le fcntl :" << pollfdmap[0].fd << std::endl;
    std::cout << "Client socket juste avant le fcntl :" << client_socket << std::endl;

    if (client_socket == -1)
    {
        std::cerr << "Error accepting connection" << std::endl;
        // throw exception("Error accepting connection")
        exit(1);
    }
    if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Error setting connection to non-blocking" << std::endl;
        // throw exception("Error setting connection to non-blocking")
        exit(1);
    }
    // std::cout << "New connection on client_socket: " << client_socket << std::endl;
    // std::cout << "User address: " << inet_ntoa(user_addr.sin_addr) << std::endl;
    // std::cout << "User port: " << ntohs(user_addr.sin_port) << std::endl;
    // std::cout << "User test: " << user_addr.sin_family << std::endl;

    std::cout << "client socket :" << client_socket << std::endl;

    // User user(client_socket);
    // std::cout << "Number of users: " << _nb_of_users << std::endl;
    // clientmap[client_socket] = user;
    pollfdmap.push_back(pollfd());
    pollfdmap.back().fd = client_socket;
    pollfdmap.back().events = POLLIN;
    // _nb_of_users++;
    // std::cout << "Number of users: " << _nb_of_users << std::endl;
    // std::cout << "User : " << clientmap[client_socket].get_fd() << std::endl;
}

Server::~Server()
{
}
