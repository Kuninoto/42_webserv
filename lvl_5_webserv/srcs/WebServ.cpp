#include "WebServ.hpp"
#include "utils.hpp"

#include <signal.h>
#include <iostream>
#include <fstream>

extern bool g_stopServer;

static void closeServer(int signum) {
	(void) signum;
	std::cout << "\n\nClosing server..." << std::endl;
	g_stopServer = true;
}

WebServ::WebServ(uint16_t port) : port(port) {
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, closeServer);
};

WebServ::~WebServ(void){};

void WebServ::parseConfigFile(std::string fileName)
{
	std::string line;
	std::ifstream file(fileName, std::ifstream::in);
	bool foundServer;
	
	while (std::getline(file, line))
	{
		if (line.find("{") != std::string::npos)
		{
			std::vector<std::string> splitted = splitStr(line, " ");

			// if not valid identifier
			if (splitted.at(0) != "server")
				throw ("not valid config file");
			
		}
	}
	
}


uint16_t WebServ::getPort(void) const {
	return this->port;
}
