#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <map>
#include "libwebserv.hpp"

class WebServ {
	public:
		WebServ(std::string filename);
		~WebServ(void);

		void addParam(const std::string& param, const std::string& value)
		{ params[param] = value; }
	
		void bootServers(void);
		void runServers(void);

		std::vector<Server> parseConfigFile(std::string filename);

		class ParserException : public std::exception {
    		public:
				std::string s;
        		ParserException(std::string ss) : s(ss) {};
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
		
		Server& getServerByName(const std::string &buffer, Server& default_server);

		std::map<std::string, std::string> params;

		struct pollfd *pollfds;

		size_t reserved_sockets, max_fds, open_fds;

		void readLocationBlock(Lexer& lexer, Token& token);
		
		locationPair parseLocation(const std::map<std::string, std::string>& lexerParameters,
                                                 std::string& locationPath);
};

#endif // WEBSERV_HPP