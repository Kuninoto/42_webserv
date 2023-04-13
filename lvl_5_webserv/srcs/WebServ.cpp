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
static int parsePortNumber(const std::string& port_as_str)
{
	unsigned long temp = strtoul(port_as_str.c_str(), NULL, 10);
	bool strtoul_success = (temp == UINT64_MAX && errno == ERANGE) ? false : true;

	if (port_as_str.find_first_not_of("0123456789") != std::string::npos 
	|| !strtoul_success || temp > UINT16_MAX)
		return -1;
	return (int)temp;
}

void WebServ::parseConfigFile(std::string fileName)
{
	std::ifstream file(fileName.c_str(), std::ifstream::in);
	bool inside = false;

	if (!file.is_open())
		throw FailedToOpenFile();

	std::string buffer;
	while (std::getline(file, buffer))
	{
		if (buffer.find_first_not_of(" \n") == std::string::npos)
			continue ;
		if (buffer.find("{") != std::string::npos && inside != true)
		{
			std::vector<std::string> splitted = splitStr(buffer, ' ');

			std::cout << "\"" << splitted.at(0) << "\"" << std::endl;

			// if not valid identifier
			if (splitted.at(0) != "server")
				throw NoServerTagException();
			inside = true;
		}
		else if (buffer.find("}") != std::string::npos && inside == true)
		{
			// The event scope was closed
			inside = false;
		}
		else if (inside == true)
		{
			// inside server {}
			std::map<std::string, std::string> test;
			std::vector<std::string> splitted2 = splitStr(buffer, ' ');
			if (splitted2.size() < 2)
				throw KeywordWithoutValueException(); 
			splitted2.at(1).erase(splitted2.at(1).length() - 1);
			if (splitted2.at(0) == "listen")
			{
				std::cout << "PORT AS STR = " << "\"" << splitted2.at(1) << "\"" << std::endl;
				int temp_port = parsePortNumber(splitted2.at(1));
				if (temp_port == -1)
					throw InvalidPortNumberException();
				this->ports.push_back(temp_port);
			}
			else if (splitted2.at(0) == "root"
			|| splitted2.at(0) == "index"
			|| splitted2.at(0) == "server_name")
				test[splitted2.at(0)] = splitted2.at(1);
			// else if (splitted2.at(0) == "location")
			// {
			// 	//deal with location
			// }
			else
			{
				std::cout << "Invalid tag = " << "\"" << splitted2.at(0) << "\"" << std::endl;
				throw UnknownTagException();
			}
		}
		else
		{
			std::cout << "mistakes were made" << std::endl;
			// ! There is a mistake in the file. Error handling
		}
	}
}

const std::vector<uint16_t>& WebServ::getPorts(void) const {
	return this->ports;
}
