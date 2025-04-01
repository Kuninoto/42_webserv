#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <unistd.h>

#include <cstring>
#include <ctime>
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
#define RS500 "500 Internal Server Error"
#define RS501 "501 Not Implemented"
#define RS505 "505 HTTP Version Not Supported"
#define RS508 "508 Loop Detected"

#define TIMEOUT_TIME_SECONDS 90

class Client {
   public:
    Client(Server server, int fd);
    std::string getRequest(void) { return this->request; };
    void setRequest(const char* chunk, size_t bufferLength);

    void response(void);
    int getFd(void) const { return this->fd; };
    Server& getTargetServer(void) { return this->server; };
    void setTargetServer(Server& server) { this->server = server; };

    bool preparedToSend(void) {
        return !(this->request_sent || this->request.find(REQUEST_DELIMITER) == std::string::npos);
    };

    bool timeout(void) {
        return std::time(NULL) - this->last_request > TIMEOUT_TIME_SECONDS;
    };

    std::map<std::string, std::string> headers;

    class ClientException : public std::exception {
       public:
        std::string s;
        ClientException(std::string ss) : s(ss){};
        ~ClientException() throw() {}

        virtual const char* what() const throw() {
            return s.c_str();
        };
    };

   private:
    Server server;

    int fd;

    bool request_sent;

    std::string method;
    std::string request;
    std::string uri_target;
    std::string requestBody;
    size_t bodyLength;
    time_t last_request;

    void parseRequest(void);
    void resolveLocation(std::string& root, std::string& uri, size_t safety_cap);
    void resolveResponse(std::string& root, std::string& uri, const location_t& targetLocation);
    void sendDirectoryListing(std::string uri);
    void sendGetResponse(std::string uri);
    void sendErrorCode(std::string code);
    void handleGetRequest(std::string& root, std::string& uri, const location_t& targetLocation);
    void handlePostRequest(std::string& root, std::string& uri, const location_t& targetLocation);
    void handleDeleteRequest(std::string& root, std::string& uri);
    std::vector<std::string> createEnvVars(const std::string& serverRoot, std::string uri, const location_t& targetLocation);
};

#endif  // CLIENT_HPP