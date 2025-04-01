#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <map>

#include "libwebserv.hpp"

#define MAX_PENDING_CONNECTIONS 10
#define POLL_FAIL "fatal: poll() failed"
#define ACCEPT_FAIL "fatal: accept() failed"

class WebServ {
   public:
    WebServ(const std::string& configFile);
    ~WebServ(void);

    void addParam(const std::string& param, const std::string& value) { params[param] = value; }

    void bootServers(void);
    void runServers(void);

    std::vector<Server> parseConfigFile(const std::string& filename);

    class ParserException : public std::exception {
       public:
        std::string s;
        ParserException(std::string ss) : s(ss){};
        ~ParserException() throw() {}

        virtual const char* what() const throw() {
            return s.c_str();
        };
    };

   private:
    std::vector<Server> servers;
    std::vector<Client> clients;

    void duplicateServers(void);
    size_t getServerUsedSockets(void);

    Server& getServerByName(const std::string& buffer, Server& default_server);
    void acceptClientConnection(int idx);
    void closeClientConnection(int idx, int clientFdIdx);
    bool isFdAServer(int fd);

    std::map<std::string, std::string> params;

    std::vector<struct pollfd> pollfds;

    size_t reserved_sockets;
};

#endif  // WEBSERV_HPP