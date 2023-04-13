#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <stdint.h>
# include <string>
# include <map>
# include <signal.h>
# include <iostream>
# include <fstream>
# include "utils.hpp"

class WebServ {
	public:
		WebServ(void);
		~WebServ(void);

		const std::vector<uint16_t>& getPorts(void) const;

		void parseConfigFile(std::string file);

		class NoServerTagException : public std::exception {
			public:
				virtual const char* what() const throw() {
					return "no server keyword";
				}
		};

		class FailedToOpenFile : public std::exception {
			public:
				virtual const char* what() const throw() {
					return "couldn't open the file";
				}
		};

		class UnknownTagException : public std::exception {
			public:
				virtual const char* what() const throw() {
					return "unknown keyword";
				}
		};

		class KeywordWithoutValueException : public std::exception {
			public:
				virtual const char* what() const throw() {
					return "keyword without value";
				}
		};

		class InvalidPortNumberException : public std::exception {
			public:
				virtual const char* what() const throw() {
					return "invalid port number";
				}
		};

	private:
		std::vector<uint16_t> ports;
		const std::map<int, std::string> error_messages;
};

#endif // WEBSERV_HPP