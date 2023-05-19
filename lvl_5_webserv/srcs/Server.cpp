#include "Server.hpp"

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <fstream>
#include <sstream>
#include <sys/socket.h>

#include "WebServ.hpp"

/**
 * @brief Validates the ClientMaxBodySize converting the str to its correspondent integer
 * @param str string representation of the ClientMaxBodySize number
 * @return On success: ClientMaxBodySize number
 * On error: throws an Exception
 */
static size_t parseClientMaxBodySize(const std::string& str) {
    unsigned long n = strtoul(str.c_str(), NULL, 10);
    bool strtoul_success = (n == UINT64_MAX && errno == ERANGE) ? false : true;

    if (str.find_first_not_of("0123456789") != std::string::npos || !strtoul_success)
        throw WebServ::ParserException("invalid client_max_body_size value \"" + str + "\"");
    return n;
}

/**
 * @brief Validates the port converting the str to its correspondent integer
 * @param port_as_str string representation of the port number
 * @return On success: port number as a string
 * On error: throws an Exception
 */
static std::string parsePortNumber(const std::string& port_as_str) {
    unsigned long temp = strtoul(port_as_str.c_str(), NULL, 10);
    bool strtoul_success = (temp == UINT64_MAX && errno == ERANGE) ? false : true;

    if (port_as_str.find_first_not_of("0123456789") != std::string::npos
    || !strtoul_success || temp > UINT16_MAX)
        throw WebServ::ParserException("invalid port number \"" + port_as_str + "\"");
    return port_as_str;
}

Server::Server(std::map<std::string, std::string>& parameters) {
    const static std::string mustHaveKeyWords[] = {
        "listen",
        "host",
        "error_page",
        "root",
        "index",
        "client_max_body_size"
    };
    
    const static std::string forbiddenKeyWords[] = {
        "upload_to",
        "cgi_path",
        "cgi_ext",
        "allow_methods",
        "auto_index",
        "try_file"
    };

    for (size_t i = 0; i < 6; i += 1) {
        if (parameters.count(mustHaveKeyWords[i]) == 0)
            throw WebServ::ParserException("no provided value for " + mustHaveKeyWords[i]);
    }

    for (size_t i = 0; i < 6; i += 1) {
        if (parameters.count(forbiddenKeyWords[i]) > 0)
            throw WebServ::ParserException("server block cannot have " + forbiddenKeyWords[i] + " directive");
    }

    this->port = parsePortNumber(parameters["listen"]);
    this->serverName = parameters["server_name"];
    this->host = parameters["host"];

    if (parameters.count("root") > 0) {
        if (*(parameters["root"].end() - 1) != '/')
            parameters["root"] += "/";
    }
    this->root = parameters["root"];

    const std::string& temp = parameters["error_page"];
    if (access(std::string(this->root + temp.c_str()).c_str(), R_OK) != 0) {
        throw WebServ::ParserException("invalid error_page \"" + temp + "\"\n" + "expanded to -> \"" + (this->root + temp.c_str()) + "\"");
    }

    this->errorPagePath = temp;
    this->index = parameters["index"];
    this->clientMaxBodySize = parseClientMaxBodySize(parameters["client_max_body_size"]);
    this->errorResponse = createErrorResponse();
    this->isDefaultServer = true;
}

Server::~Server(void) {};

std::string Server::createErrorResponse(void) {
    std::string path_to_error_page = this->root + this->errorPagePath;
    std::ifstream error_page_file(path_to_error_page.c_str(), std::ios::binary | std::ios::in);

    std::string error_response =
        "HTTP/1.1 404 Not Found\n"
        "Date: " + getTimeStamp() + "\n"
        "Server: Server: 42_Webserv/1.0 (Linux)\n" +
        "Content-Type: " + getFileType(path_to_error_page) + "; charset=UTF-8\n" +
        "Content-Length: " + getFileSize(path_to_error_page) + "\n\n";

    error_response.append((std::istreambuf_iterator<char>(error_page_file)),
                           std::istreambuf_iterator<char>());
    return error_response;
}

void Server::createSocket(void) {
    this->socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socketFd == -1)
        throw std::runtime_error(SOCKET_OPEN_ERR);
}

std::ostream& operator<<(std::ostream& stream, Server& sv) {
    stream << "SERVER = \"" << sv.getServerName() << "\"\n"
           << "PORT = \"" << sv.getPort() << "\"\n"
           << "HOST = \"" << sv.getHost() << "\"\n"
           << "ERROR_PAGE = \"" << sv.getErrorPagePath() << "\"\n"
           << "ROOT = \"" << sv.getRoot() << "\"\n"
           << "INDEX = \"" << sv.getIndex() << "\"\n"
           << "CLIENT_MAX_BODY_SIZE = \"" << sv.getMaxBodySize() << "\"\n"
           << "LOCATIONS --------------" << '\n';

    locationMap::const_iterator itr;
    for (itr = sv.getLocations().begin(); itr != sv.getLocations().end(); itr++) {
        stream << '\n'
               << "location \"" << itr->first << "\"\n"
               << "root = \"" << itr->second.root << "\"\n"
               << "ALLOW_METHODS = ";
        for (size_t i = 0; i < itr->second.allowed_methods.size(); i++)
            stream << "\"" << itr->second.allowed_methods.at(i) << "\" ";
        stream << '\n'
               << "AUTO_INDEX = \"" << (itr->second.auto_index ? "yes" : "no") << "\"\n"
               << "RETURN = \"" << itr->second.redirect << "\"\n"
               << "CGI_PATH = \"" << itr->second.cgi_path << "\"\n"
               << "CGI_EXT = \"" << itr->second.cgi_ext << "\"\n";
    }
    return stream;
}
