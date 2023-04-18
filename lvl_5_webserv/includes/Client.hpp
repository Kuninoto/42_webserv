#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>

#include "utils.hpp"
#include "WebServ.hpp"

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
        Client(WebServ server, int fd);

        e_methods getMethod(void) const { return this->method; };

        void setRequest(std::string request);
        
        void response();

    private:
        WebServ server;

        int fd;
        bool request_sent;

        e_methods method;
        e_types type;
        std::string request;
        std::string page;
        
        void parseRequest();
        void responseFavIcon();
        std::string getPathToPage();
};

#endif // CLIENT_HPP