#ifndef SERVER_HPP
# define SERVER_HPP

# include <stdint.h>
# include <string>
# include <map>

class Server {
    public:
        Server(std::map<std::string, std::string>& parameters);
		~Server(void) {};

        uint16_t getPort(void) const { return this->port; };
        const std::string& getHost(void) const { return this->host; };
        const std::string& getIndex(void) const { return this->index; };
        const std::string& getServerName(void) const { return this->serverName; };
        const std::string& getErrorPagePath(void) const { return this->errorPagePath; };
        const std::string& getRoot(void) const { return this->root; };
        size_t getMaxBodySize(void) const { return this->clientMaxBodySize; };

    private:
        // 8080
        uint16_t port;
        // localhost
        std::string serverName;
        // 127.0.0.1
        std::string host;
        // 404 pages/error/404.html
        std::string errorPagePath;
        std::string root;
        std::string index;
        size_t clientMaxBodySize;

        std::map<std::string, std::string> parameters;

};

std::ostream &operator<<(std::ostream &stream, Server& sv);

#endif // SERVER_HPP