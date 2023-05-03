#ifndef LISTENER_HPP
# define LISTENER_HPP

# include <string>
# include <map>
# include <sys/socket.h>
# include "Server.hpp"
# include "Response.hpp"
# include "Logger.hpp"

# define READ_BUFFER_SIZE 1024

class Connection
{
public:
    Connection(Server &server, int socket);
    Connection(Server &server, int socket, struct sockaddr_in address);

private:
    Connection();

    Server _server;
    int _socket;
    struct sockaddr_in _address;
    std::string _request;
    std::string _response;
    size_t send_pos;

public:
    ssize_t recv();
    ssize_t send();
};

#endif