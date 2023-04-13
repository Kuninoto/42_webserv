#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <stdint.h>
# include <string>
# include <map>

class WebServ {
	public:
		WebServ(void);
		~WebServ(void);

		uint16_t getPort(void) const;

		void parseConfigFile(std::string file);

		class NoServerTagException : public std::exception {
			public:
				virtual const char* what() const throw() {
					return "Invalid config file: no server keyword";
				}
		};

		class UnknownTagException : public std::exception {
			public:
				virtual const char* what() const throw() {
					return "Invalid config file: unknown keyword";
				}
		};

	private:
		uint32_t port;
		const std::map<int, std::string> error_messages;
};

#endif // WEBSERV_HPP