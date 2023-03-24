
#define PORT 8080

#include <netinet/in.h>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "macros.hpp"

int main(int argc, char const* argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	int status, valread, client_fd;
	struct sockaddr_in serv_addr;
	std::string message;
	std::getline(std::cin, message);
	char buffer[1024] = { 0 };
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		LOG_ERROR("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
		<= 0) {
		LOG_ERROR("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((status = connect(client_fd, (struct sockaddr*)&serv_addr,
						   sizeof(serv_addr)))
		< 0) {
		LOG_ERROR("\nConnection Failed \n");
		return -1;
	}
	send(client_fd, message.c_str(), message.size(), 0);
	LOG_DEBUG("Message sent\n");
	valread = read(client_fd, buffer, 1024);
	printf("%s\n", buffer);
	UNUSED(valread);

	// closing the connected socket
	close(client_fd);
	return 0;
}
