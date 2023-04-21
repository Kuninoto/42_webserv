#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sstream>
#include <fstream>
#include "Server.hpp"
#include "WebServ.hpp"

static size_t parseClientMaxBodySize(const std::string& str)
{
    size_t n = strtoul(str.c_str(), NULL, 10);
	bool strtoul_success = (n == UINT64_MAX && errno == ERANGE) ? false : true;

	if (str.find_first_not_of("0123456789") != std::string::npos 
	|| !strtoul_success)
		throw WebServ::ParserException("invalid client_max_body_size value");
	return n;
}

/**
 * @brief Converts the string representation of the port to its correspondent integer
 * @param port_as_str string representation of the port number
 * @return On success: port number
 * On error: -1 (invalid port number)
 */
static std::string parsePortNumber(const std::string& port_as_str)
{
	unsigned long temp = strtoul(port_as_str.c_str(), NULL, 10);
	bool strtoul_success = (temp == UINT64_MAX && errno == ERANGE) ? false : true;

	if (port_as_str.find_first_not_of("0123456789") != std::string::npos 
	|| !strtoul_success || temp > UINT16_MAX)
		throw WebServ::ParserException("invalid port number");
	return port_as_str;
}

Server::Server(std::map<std::string, std::string>& parameters)
{
    const static std::string mustHaveKeyWords[] = {
        "listen",
        "host",
        "error_page",
        "root",
        "index",
        "client_max_body_size"
    };

    for (size_t i = 0; i < 6; i += 1) {
        if (parameters.count(mustHaveKeyWords[i]) == 0)
            throw WebServ::ParserException("no provided value for " + mustHaveKeyWords[i]);
    }

    this->port = parsePortNumber(parameters["listen"]);
    this->serverName = parameters["server_name"];
    this->host = parameters["host"];

    std::string& temp = parameters["error_page"];
    if (!access(temp.c_str(), F_OK|R_OK))
        throw WebServ::ParserException("invalid error_page");

    this->errorPagePath = temp;
    this->root = parameters["root"];
    this->index = parameters["index"];
    this->clientMaxBodySize = parseClientMaxBodySize(parameters["client_max_body_size"]);

    this->createErrorResponse();
}

void Server::createErrorResponse()
{
    std::string path_to_error_page = this->root + this->errorPagePath;

    std::ifstream file(path_to_error_page.c_str(), std::ios::binary | std::ios::in);

    this->error_response = "HTTP/1.1 400 Not Found\nContent-Type: " + getFileType(path_to_error_page) + "; charset=UTC-8\nContent-Length: " + getFileSize(path_to_error_page) + "\n\n";
    
    this->error_response.append((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void Server::createSocket(void)
{
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socket_fd == -1)
		throw std::out_of_range(SOCKET_OPEN_ERR);
}

std::ostream &operator<<(std::ostream &stream, Server& sv)
{
	stream << "SERVER = " << sv.getServerName() << '\n'
           << "PORT = " << sv.getPort() << '\n'
           << "HOST = " << sv.getHost() << '\n'
           << "ERROR_PAGE = " << sv.getErrorPagePath() << '\n'
           << "ROOT = " << sv.getRoot() << '\n'
           << "INDEX = " << sv.getIndex() << '\n'
           << "CLIENT_MAX_BODY_SIZE = " << sv.getMaxBodySize() << std::endl;
	return stream;
}
