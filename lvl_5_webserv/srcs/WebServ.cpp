#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <stdlib.h>
#include <signal.h>
#include "WebServ.hpp"

extern bool g_stopServer;

static void closeServer(int signum)
{
	(void) signum;
	std::cout << "\n\nClosing server..." << std::endl;
	g_stopServer = true;
}

WebServ::WebServ(void)
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, closeServer);
};

WebServ::~WebServ(void){};

/**
 * @brief Converts the string representation of the port to its correspondent integer
 * @param port_as_str string representation of the port number
 * @return On success: port number
 * On error: -1 (invalid port number)
 */
static int parse_port_number(const std::string& port_as_str)
{
	unsigned long temp = strtoul(port_as_str.c_str(), NULL, 10);
	bool strtoul_success = (temp == UINT64_MAX && errno == ERANGE) ? false : true;

	if (port_as_str.find_first_not_of("0123456789") != std::string::npos || !strtoul_success || temp > UINT16_MAX)
		return -1;
	return (int)temp;
}

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
			std::vector<std::string> splitted = splitStr(buffer, ' ');

			//std::cout << "\"" << splitted.at(0) << "\"" << std::endl;
			// if not valid identifier
			if (splitted.at(0) != "server")
				throw NoServerTagException();

			// inside server {}
			std::map<std::string, std::string> test;
			while (std::getline(file, buffer))
			{
				std::cout << "BUFFER = " << buffer << std::endl;
				std::vector<std::string> splitted2 = splitStr(buffer, ' ');
				std::cout << "a: " << splitted2.at(1) << std::endl;
				std::string temp;

				if (splitted2.at(0) == "listen")
				{
					temp = splitted2.at(1);
					test[splitted2.at(0)] = temp;

					temp.clear();
					int port_temp = parse_port_number(test["listen"]); 
					if (port_temp == -1)
						std::cout << "FAIL" << std::endl;
					this->port = parse_port_number(test["listen"]);
					std::cout << "LISTEN VALUE = " << "\" "<< test["listen"] << "\" " << std::endl;
					std::cout << "PORT = " << this->port << std::endl;
					exit(0);
					break ;
				}
				else
					throw UnknownTagException();
			}
		}
		break;
	}
	
}

uint16_t WebServ::getPort(void) const {
	return this->port;
}
