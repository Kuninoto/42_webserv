#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <string>
# include <map>
# include "Server.hpp"
# include "utils.hpp"

class WebServ {
	public:
		WebServ(void);
		~WebServ(void);

		void addParam(const std::string& param, const std::string& value)
		{ params[param] = value; }
	
		void parseConfigFile(const std::string& filename);

		class NoServerTagException : public std::exception {
			public:
				virtual const char* what() const throw() {
					return "no server keyword";
				}
		};

	private:
		std::vector<Server> servers;
		std::map<std::string, std::string> params;
	//	const std::map<int, std::string> error_messages;
};

#endif // WEBSERV_HPP