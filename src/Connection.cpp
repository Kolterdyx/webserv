#include "Connection.hpp"

Connection::Connection(Server &server, int socket) : _socket(socket), send_pos(0) {
	memset(&_address, 0, sizeof(_address));
	_server = server;
}

Connection::Connection(Server &server, int socket, struct sockaddr_in address)
	: _socket(socket), send_pos(0), _address(address) {
	_server = server;
}

ssize_t Connection::recv() {
	char buffer[READ_BUFFER_SIZE + 1] = {0};
	ssize_t valread = 1;

	valread = recv(_socket, buffer, READ_BUFFER_SIZE, 0);
	if (valread > 0) {
		_request += buffer;
	}

	// if (completeRequest()) {
		Response response = getResponse(_request, 0);   // TODO siempre se pasa 0, para que el address?
		_response = response.toString();
		logger.log("Response: " + response.getStatusString(), 9);
	// }
    return valread;
}

ssize_t Connection::send() {
	if (_response.empty()) {
		return -1;
	}

	ssize_t valsend;
	valsend = send(_socket, &_response.c_str()[send_pos], _response.size() - send_pos, 0);
	if (valsend > 0)
		send_pos += valsend;
    return valsend;
}


/*
    char	buffer[RECV_SIZE] = {0};
	int		ret;

	ret = ::recv(socket, buffer, RECV_SIZE - 1, 0);

	if (ret == 0 || ret == -1)
	{
		this->close(socket);
		if (!ret)
			std::cout << "\rConnection was closed by client.\n" << std::endl;
		else
			std::cout << "\rRead error, closing connection.\n" << std::endl;
		return (-1);
	}

	_requests[socket] += std::string(buffer);

	size_t	i = _requests[socket].find("\r\n\r\n");
	if (i != std::string::npos)
	{
		if (_requests[socket].find("Content-Length: ") == std::string::npos)
		{
			if (_requests[socket].find("Transfer-Encoding: chunked") != std::string::npos)
			{
				if (checkEnd(_requests[socket], "0\r\n\r\n") == 0)
					return (0);
				else
					return (1);
			}
			else
				return (0);
		}

		size_t	len = std::atoi(_requests[socket].substr(_requests[socket].find("Content-Length: ") + 16, 10).c_str());
		if (_requests[socket].size() >= len + i + 4)
			return (0);
		else
			return (1);
	}

	return (1);
*/