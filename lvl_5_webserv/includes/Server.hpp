#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#define SOCKET_OPEN_ERR "fatal: failed to open socket"

typedef struct location_s {
    std::string root;
    std::vector<std::string> allowed_methods;
    std::string redirect;
    bool auto_index;
    std::string try_file;
    bool hasCGI;
    std::string cgi_path;
    std::string cgi_ext;
    std::string uploadTo;
}               location_t;

typedef std::map<std::string, location_t> locationMap;
typedef std::pair<std::string, location_t> locationPair;

class Server {
   public:
    Server(std::map<std::string, std::string>& parameters);
    ~Server(void);

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

    void addLocation(locationPair newlocationPair) { this->locations.insert(newlocationPair); };
    void createSocket(void);

    bool isDefaultServer;  // default for host:port


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

    locationMap locations;

    std::string createErrorResponse(void);
};

std::ostream& operator<<(std::ostream& stream, Server& sv);

#endif  // SERVER_HPP