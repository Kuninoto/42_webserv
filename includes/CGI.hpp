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

#include "libwebserv.hpp"

class CGI {
   public:
    CGI(const std::string& cgi_ext, const std::string& request,
        const std::vector<std::string>& envVars, size_t bodyLength,
        const std::string& uploadTo);
    ~CGI(void);

    std::string response;

    class CGIException : public std::exception {
       public:
        std::string message;
        CGIException(std::string message) : message("CGI error: " + message){};
        ~CGIException() throw(){};
        virtual const char* what() const throw() {
            return message.c_str();
        };
    };

   private:
    void runScript(void);
    void createArgvAndEnvp(const std::vector<std::string>& envVars);

    const std::string& request;
    const size_t bodyLength;
    const std::string& uploadTo;
    char** argv;
    char** envp;
    std::string cgi_path;
    std::string cgi_ext;
    std::vector<std::string> params;
    std::string runner;

    CGI(void);
};

#endif  // CGI_HPP