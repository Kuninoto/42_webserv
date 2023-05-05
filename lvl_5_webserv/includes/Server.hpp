#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdint.h>
#include <sys/socket.h>

#include <map>
#include <sstream>
#include <string>
#include <vector>

#define SOCKET_OPEN_ERR "fatal: failed to open socket"

typedef struct location_s {
    std::string root;
    std::vector<std::string> allowed_methods;
    std::string redirect;
    bool auto_index;
    std::string try_file;
    std::string cgi_path;
    std::string cgi_ext;
} location_t;

typedef std::map<std::string, location_t> locationMap;
typedef std::pair<std::string, location_t> locationPair;

class Server {
   public:
    Server(std::map<std::string, std::string>& parameters);
    ~Server(void){};

    const std::string& getPort(void) const { return this->port; };
    const std::string& getHost(void) const { return this->host; };
    const std::string& getIndex(void) const { return this->index; };
    const std::string& getServerName(void) const { return this->serverName; };
    const std::string& getErrorPagePath(void) const { return this->errorPagePath; };
    const std::string& getRoot(void) const { return this->root; };
    const locationMap& getLocations() const { return this->locations; };
    std::string getErrorResponse(void) { return this->errorResponse; };
    size_t getMaxBodySize(void) const { return this->clientMaxBodySize; };
    int getSocketFd(void) const { return this->socketFd; };
    bool getSkipBind(void) { return this->skip_bind; };

    void setSkipBind(void) { this->skip_bind = true; };

    void addLocation(locationPair locationPair);
    void createSocket(void);

    bool isDefaultServer;  // default for host:port

    locationMap locations;

   private:
    std::string port;
    std::string host;
    std::string serverName;
    std::string index;
    std::string errorPagePath;
    std::string root;
    size_t clientMaxBodySize;
    std::string errorResponse;
    int socketFd;

    bool skip_bind;

    std::string createErrorResponse(void);
};

std::ostream& operator<<(std::ostream& stream, Server& sv);

#endif  // SERVER_HPP