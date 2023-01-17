
#include "webserv.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: ./webserv [config file]" << std::endl;
		return (1);
	}
	Config config(argv[1]);
	Webserv webserv(config);
	webserv.run();
	return (0);
}

