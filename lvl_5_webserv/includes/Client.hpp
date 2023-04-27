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

        class ClientException : public std::exception {
    		public:
				std::string s;
        		ClientException(std::string ss) : s(ss) {};
				~ClientException() throw() {}
    
    			virtual const char* what() const throw() {
        			return s.c_str();
    			};
		};

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
        void sendDirectoryListing(std::string path);
        void sendResponse(std::string path, std::string code);
        void sendErrorCode(std::string code);
        std::string resolvePathAndLocation(); 

};

#endif // CLIENT_HPP