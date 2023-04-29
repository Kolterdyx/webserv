#ifndef LISTENER_HPP
# define LISTENER_HPP

# include <cstring>
# include <cstdlib>
# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <poll.h>
# include "Route.hpp"
# include "Logger.hpp"

# define DEFAULT_IP "0.0.0.0"
# define DEFAULT_PORT 8080
# define DEFAULT_BACKLOG 100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

class Listener
{
public:
    Listener();
    Listener(
        std::string ip, // Ip of the socket
        short port,     // Port of the socket
        int backlog=DEFAULT_BACKLOG     // Maximum length for the queue of pending connections.
    );

private:
    int _socket;
    int _backlog;

public:
    int getSocket() const;
    int newConnection(sockaddr *addr=NULL);
};

#endif
