#ifndef SERVER_HPP
# define SERVER_HPP

# include <stdint.h>
# include <string>

class Server {
    public:
        Server(void) {};
		~Server(void) {};

        uint16_t getPort(void) const { return this->port; };
        const std::string& getHost(void) const { return this->host; };
        const std::string& getServerName(void) const { return this->serverName; };
        size_t getMaxBodySize(void) const { return this->maxBodySize; };

		void setPort(uint16_t port) { this->port = port; };
        void setHost(std::string& host) { this->host = host; };
        void setServerName(std::string& serverName) { this->serverName = serverName; };
        void setMaxBodySize(size_t maxBodySize) { this->maxBodySize = maxBodySize; };

    private:
        // 8080
        uint16_t port;
        // 127.0.0.1
        std::string host;
        // localhost
        std::string	serverName;
        // 404 pages/error/404.html
        std::string	errorPages;
        size_t maxBodySize;
};

#endif // SERVER_HPP