#ifndef CGI_HPP
#define CGI_HPP

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <map>
#include <string>
#include <vector>

#include "libwebserv.hpp"

class CGI {
   public:
    CGI(const std::string& cgi_ext, const std::string& request);
    ~CGI(void);

    std::string response;

   private:
    void runCGI(void);
    void runScript(void);
    void createArgs(void);
    void getEnvVars(void);
    void checkVars(void);

    char** args;
    const std::string& request;
    std::string cgi_path;
    std::string cgi_ext;
    std::vector<std::string> params;
    std::map<std::string, std::string> envVars;
    std::string runner;
};

class CGIException : public std::exception {
   public:
    std::string message;
    CGIException(std::string message) : message("CGI error: " + message) {};
    ~CGIException() throw() {};
    virtual const char* what() const throw() {
        return message.c_str();
    };
};

#endif  // CGI_HPP