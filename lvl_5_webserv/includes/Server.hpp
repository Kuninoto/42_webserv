#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sstream>

#define SOCKET_OPEN_ERR "fatal: failed to open socket"
#define SETSOCKETOPT_ERR "fatal: settockopt() failed"

typedef struct location_s {
    std::string root;
    std::vector<std::string> allowed_methods;
    std::string redirect;
    bool auto_index;
    std::string try_file;
    std::string cgi_path;
    std::string cgi_ext;
}               location_t;

typedef std::map<std::string, location_t> locationMap;
typedef std::pair<std::string, location_t> locationPair;

class Server {
    public:
        Server(std::map<std::string, std::string>& parameters);
		~Server(void) {};

        const std::string& getPort(void) const { return this->port; };
        const std::string& getHost(void) const { return this->host; };
        const std::string& getIndex(void) const { return this->index; };
        const std::string& getServerName(void) const { return this->serverName; };
        const std::string& getErrorPagePath(void) const { return this->errorPagePath; };
        const std::string& getRoot(void) const { return this->root; };
        const locationMap& getLocations() const { return this->locations; };
        std::string getErrorResponse(void){ return this->error_response; };
        size_t getMaxBodySize(void) const { return this->clientMaxBodySize; };
        bool getSkipBind(void) { return this->skip_bind; };
		int getSocketFd(void) const { return this->socket_fd; };
        void addLocation(std::pair<std::string, location_t> locationPair);
		void createSocket(void);

        void setSkipBind(void) { this->skip_bind = true; };

        locationMap locations;

    private:
        // 8080
        std::string port;
        // localhost
        std::string serverName;
        // 127.0.0.1
        std::string host;
        // 404 pages/error/404.html
        std::string errorPagePath;
        std::string root;
        std::string index;
        size_t clientMaxBodySize;

        bool skip_bind;

        std::string error_response;

		int	socket_fd;

        void createErrorResponse();
};

std::ostream &operator<<(std::ostream &stream, Server& sv);

#endif // SERVER_HPP