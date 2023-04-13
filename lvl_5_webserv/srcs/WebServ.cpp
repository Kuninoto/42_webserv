#include "WebServ.hpp"

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
	bool inside = false;

	if (!file.is_open())
		throw FailedToOpenFile();

	std::string buffer;
	while (std::getline(file, buffer))
	{
		// Check if string is empty
		if (emptyStr(buffer))
			continue; 
		else if (buffer.find("{") != std::string::npos && inside != true)
		{
			std::vector<std::string> splitted = splitStr(buffer, " ");

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
			std::cout << "BUFFER = " << buffer << std::endl;

			// SPLIT IS NOT WORKING PROPERLY
			std::vector<std::string> splitted2 = splitStr(buffer, " ");

			// Verify if it's a valid option
			if (splitted2.at(0) == "listen")
				test[splitted2.at(0)] = splitted2.at(1);
			else if (splitted2.at(0) == "root")
				test[splitted2.at(0)] = splitted2.at(1);
			else if (splitted2.at(0) == "index")
				test[splitted2.at(0)] = splitted2.at(1);
			else if (splitted2.at(0) == "server_name")
				test[splitted2.at(0)] = splitted2.at(1);
			// else if (splitted2.at(0) == "location")
			// {
			// 	//deal with location
			// }
			else
				throw UnknownTagException();
		}
		else
		{
			std::cout << "mistakes were made" << std::endl;
			// ! There is a mistake in the file. Error handling
		}
	}
	
}


uint16_t WebServ::getPort(void) const {
	return this->port;
}
