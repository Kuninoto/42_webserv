#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <string>
# include <map>
# include <poll.h>

# include "Server.hpp"
# include "utils.hpp"

class WebServ {
	public:
		WebServ(void);
		~WebServ(void);

		void addParam(const std::string& param, const std::string& value)
		{ params[param] = value; }
	
		void parseConfigFile(const std::string& filename);

		void bootServers(std::vector<Server> &servers, struct pollfd **pollfds);

		class ParserException : public std::exception {
    		public:
        		ParserException(const std::string& message) : message(message) {};
    
    		virtual const char* what() const throw() {
        		return message.c_str();
    		};

    		private:
        	const std::string& message;
		};

	private:
		std::vector<Server> servers;
		std::map<std::string, std::string> params;
	//	const std::map<int, std::string> error_messages;
};

#endif // WEBSERV_HPP