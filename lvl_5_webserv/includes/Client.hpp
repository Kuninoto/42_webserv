#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>

#include "utils.hpp"

typedef enum methods {
    GET,
    POST,
    DELETE,
}           e_methods;

class Client {
    public:
        e_methods getMethod(void) const { return this->method; };

        Client(int fd);

        void setRequest(std::string request);

        void response();

    private:
        int fd;
        bool request_sent;

        std::string request;
        e_methods method;
};

#endif // CLIENT_HPP