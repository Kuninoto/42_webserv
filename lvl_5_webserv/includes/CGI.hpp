#ifndef CGI_HPP
#define CGI_HPP

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "libwebserv.hpp"

class Lexer;

class CGI {
   public:
    CGI(const std::string& cgi_path, const std::string& cgi_ext);
    ~CGI(void);

    std::string response;

   private:
    void runCGI(void);
    void runScript(void);
    void createArgs(void);
    void parseQueryString(void);
    void getEnvVars(void);
    void checkVars(void);

    char **args;
    const std::string& cgi_path;
    const std::string& cgi_ext;
    std::vector<std::string> params;
    std::map<std::string, std::string> envVars;
    std::string runner;
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