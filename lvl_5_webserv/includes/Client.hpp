#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>

#include "utils.hpp"
#include "Server.hpp"

#define REQUEST_DELIMITER "\r\n\r\n"

typedef enum methods 
{
    GET,
    POST,
    DELETE,
}           e_methods;

typedef enum types 
{
    TEXT,
    FAVICON,
    HTML,
    IMAGE
}           e_types;

class Client {
    public:
        Client(Server server, int fd);

        e_methods getMethod(void) const { return this->method; };

        void setRequest(std::string request);
        
        void response();

    private:
        Server server;

        int fd;

        bool request_sent;
        e_methods method;
        
        std::string request;
        std::string page;
        
        void parseRequest();
        void resolveResponse(std::string& root, std::string& path, size_t safety_cap);
        void responseFavIcon();
        void sendResponse(std::string path);
        std::string resolvePathAndLocation();
        void sendDirectoryListing(std::string path);


};

#endif // CLIENT_HPP