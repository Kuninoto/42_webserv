#include "utils.hpp"
#include "WebServ.hpp"

#include <signal.h>
#include <iostream>
#include <fstream>

#include <map>

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
	std::ifstream file(fileName.c_str(), std::ifstream::in);

//	if (!file.is_open())
//		return failed to open config file

	std::string buffer;
	while (std::getline(file, buffer))
	{
		if (buffer.find("{") != std::string::npos)
		{
			std::vector<std::string> splitted = splitStr(buffer, " ");

			std::cout << "\"" << splitted.at(0) << "\"" << std::endl;
			// if not valid identifier
			if (splitted.at(0) != "server")
				throw NoServerTagException();

			// inside server {}
			std::map<std::string, std::vector<std::string> > test;
			while (std::getline(file, buffer))
			{
				std::cout << "BUFFER = " << buffer << std::endl;
				// SPLIT IS NOT WORKING PROPERLY
				std::vector<std::string> splitted2 = splitStr(buffer, " ");
				std::vector<std::string> temp;

				if (splitted2.at(0) == "listen")
				{
					temp.push_back(splitted2.at(1));
					test[splitted2.at(0)] = temp;
					temp.clear();
				}
				else
					throw UnknownTagException();
			}
		}
	}
	
}


uint16_t WebServ::getPort(void) const {
	return this->port;
}
