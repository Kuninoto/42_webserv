#ifndef SERVER_HPP
# define SERVER_HPP

# include <stdint.h>
# include <string>
# include <vector>
# include <map>

typedef struct location_s {
    std::string index;
    std::string root;
    std::vector<std::string> allowed_methods;
    std::string redirect;
    bool auto_index;
    std::string cgi_path;
    std::string cgi_ext;
}               location_t;


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
        const std::string& getLocationRoot(const std::string& key) { return this->locations[key].root; };
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
        std::map<std::string, location_t> locations;
};

std::ostream &operator<<(std::ostream &stream, Server& sv);

#endif // SERVER_HPP