#include "Listener.hpp"

#include <iostream>

Listener::Listener() {
    Listener(DEFAULT_IP, DEFAULT_PORT, DEFAULT_BACKLOG);
}

Listener::Listener(std::string ip, short port, int backlog) : _backlog(backlog) {
    SA_IN server_addr;
    int opt = 1;

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
        logger.error("Failed to create socket");
        return;
    }

    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        logger.error("Failed to set socket options");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(port);

    if (bind(_socket, (SA*)&server_addr, sizeof(server_addr)) < 0) {
        logger.error("Failed to bind socket");
        return; // TODO what happens when an error. return?
    }
    if (listen(_socket, _backlog) < 0) {
        logger.error("Failed to listen on socket");
        return;
    }

    int flags = fcntl(_socket, F_SETFL, fcntl(_socket, F_GETFL) | O_NONBLOCK);
    if (flags < 0) {
        logger.error("Failed to set socket to non-blocking");
        return;
    }
}

int Listener::getSocket() const { return _socket; }

int Listener::newConnection(sockaddr *addr) {
    int client_socket;
    SA client_addr;
    socklen_t addr_size;

    client_socket = accept(_socket, &client_addr, &addr_size);
    if (addr)
        memcpy(addr, &client_addr, sizeof(client_addr));
    return client_socket;
}