#ifndef CGI_HPP
#define CGI_HPP

#include <dirent.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "libwebserv.hpp"

class Lexer;

class CGI {
   public:
    CGI(void);
    ~CGI(void);

    std::string response;

   private:
    void runCGI(void);
    bool validPath(void);
    void runScript(void);
    void setExtension(void);
    void createArgs(void);
    void parseQueryString(void);
    void getEnvVars(void);
    void checkVars(std::string method);

    char **args;
    std::vector<std::string> params;
    std::map<std::string, std::string> envVars;
    std::string method;
    std::string filePath;
    std::string extension;
    std::string runner;
    std::string error;
};

class CGIException : public std::exception {
   public:
    CGIException(const std::string error) throw() { message = new std::string(error); };
    virtual ~CGIException() throw() { delete message; }

    virtual const char *what() const throw() {
        return message->c_str();
    };

   private:
    const std::string *message;
};

#endif  // CGI_HPP