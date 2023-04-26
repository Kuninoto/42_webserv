#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <map>
# include "libwebserv.hpp"

class WebServ {
	public:
		WebServ(std::string filename);
		~WebServ(void);

		void addParam(const std::string& param, const std::string& value)
		{ params[param] = value; }
	
		void bootServers();
		void runServers();

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

		std::map<std::string, std::string> params;

		struct pollfd *pollfds;

		size_t reserved_sockets, max_fds, open_fds;


	//	const std::map<int, std::string> error_messages;
};

#endif // WEBSERV_HPP