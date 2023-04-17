#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sstream>
#include "Server.hpp"

static size_t parseClientMaxBodySize(const std::string& str)
{
    size_t n = strtoul(str.c_str(), NULL, 10);
	bool strtoul_success = (n == UINT64_MAX && errno == ERANGE) ? false : true;

	if (str.find_first_not_of("0123456789") != std::string::npos 
	|| !strtoul_success)
		throw std::invalid_argument("invalid client_max_body_size value");
	return n;
}

/**
 * @brief Converts the string representation of the port to its correspondent integer
 * @param port_as_str string representation of the port number
 * @return On success: port number
 * On error: -1 (invalid port number)
 */
static uint16_t parsePortNumber(const std::string& port_as_str)
{
	unsigned long temp = strtoul(port_as_str.c_str(), NULL, 10);
	bool strtoul_success = (temp == UINT64_MAX && errno == ERANGE) ? false : true;

	if (port_as_str.find_first_not_of("0123456789") != std::string::npos 
	|| !strtoul_success || temp > UINT16_MAX)
		throw std::invalid_argument("invalid port number");
	return temp;
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
            throw std::runtime_error("no provided value for " + mustHaveKeyWords[i]);
    }

    this->port = parsePortNumber(parameters["listen"]);
    this->serverName = parameters["server_name"];
    this->host = parameters["host"];

    std::string& temp = parameters["error_page"];
    if (!access(temp.c_str(), F_OK|R_OK));
        throw std::runtime_error("invalid error_page");

    this->errorPage = temp;
    this->root = parameters["root"];
    this->index = parameters["index"];
    this->clientMaxBodySize = parseClientMaxBodySize(parameters["client_max_body_size"]);
}
