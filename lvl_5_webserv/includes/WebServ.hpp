#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <map>

#include "libwebserv.hpp"

#define SETSOCKETOPT_ERR "fatal: settockopt() failed"
#define GETADDRINFO_ERR "fatal: getaddrinfo() failed"
#define BIND_ERR "fatal: bind() failed"
#define LISTEN_ERR "fatal: listen() failed"
#define POLL_FAIL "fatal: poll() failed"
#define ACCEPT_FAIL "fatal: accept() failed"

class WebServ {
   public:
    WebServ(std::string configFile);
    ~WebServ(void);

    void addParam(const std::string& param, const std::string& value) { params[param] = value; }

    void bootServers(void);
    void runServers(void);

    std::vector<Server> parseConfigFile(const std::string& filename);

    class ParserException : public std::exception {
       public:
        std::string message;
        ParserException(std::string message) : message(message){};
        ~ParserException() throw() {}
        virtual const char* what() const throw() {
            return message.c_str();
        };
    };

   private:
    std::vector<Server> servers;
    std::vector<Client> clients;

    void duplicateServers(void);
    size_t getNumberOfSockets(void);
    bool checkTriggeredFd(int fd);

    Server& getServerByName(const std::string& buffer, Server& default_server);

    std::map<std::string, std::string> params;

    struct pollfd* pollfds;

    size_t reserved_sockets, max_fds, open_fds;

    void readLocationBlock(Lexer& lexer, Token& token);

    locationPair parseLocation(const std::map<std::string, std::string>& lexerParameters,
                               std::string& locationPath);
};

#endif  // WEBSERV_HPP