#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <stdint.h>
# include <string>
# include <map>
# include "utils.hpp"

class WebServ {
	public:
		WebServ(void);
		~WebServ(void);

		uint16_t getPort(void) const { return this->port; };

		void setPort(uint16_t port) { this->port = port; }

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
		std::map<std::string, std::string> params;
		uint16_t port;
		const std::map<int, std::string> error_messages;
};

#endif // WEBSERV_HPP