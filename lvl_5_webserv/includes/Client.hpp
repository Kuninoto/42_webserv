#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>

#include "Server.hpp"
#include "utils.hpp"

#define REQUEST_DELIMITER "\r\n\r\n"

// RS - Response Status

#define RS200 "200 OK"
#define RS400 "400 Bad Request"
#define RS403 "403 Forbidden"
#define RS405 "405 Method Not Allowed"
#define RS413 "413 Content Too Large"
#define RS414 "414 URI Too Large"
#define RS501 "501 Not Implemented"
#define RS505 "505 HTTP Version Not Supported"
#define RS508 "508 Loop Detected"

class Client {
   public:
    Client(Server server, int fd);
    std::string getRequest(void) { return this->request; };
    void setRequest(std::string request);

    void response(void);

    int getFd(void) const { return this->fd; };
    Server& getTargetServer(void) { return this->server; };
    void setTargetServer(Server& server) { this->server = server; } ;
    bool preparedToSend(void) {
        return !(this->request_sent || this->request.find(REQUEST_DELIMITER) == std::string::npos);
    };

    class ClientException : public std::exception {
       public:
        std::string s;
        ClientException(std::string ss) : s(ss){};
        ~ClientException() throw() {}

        virtual const char* what() const throw() {
            return s.c_str();
        };
    };

    std::map<std::string, std::string> headers;

   private:
    Server server;

    int fd;

    bool request_sent;
    std::string method;

    std::string request;
    std::string uri_target;
    std::string request_content;

    void parseRequest(void);
    void resolveResponse(std::string& root, std::string& uri, size_t safety_cap);
    void responseFavIcon(void);
    void sendDirectoryListing(std::string uri);
    void sendResponse(std::string uri);
    void sendErrorCode(std::string code);
    // std::string resolvePathAndLocation(void);
};

#endif  // CLIENT_HPP