#ifndef CGI_HPP
# define CGI_HPP

# include <sys/stat.h>
# include <cstdlib>
# include <stdint.h>
# include <string>
# include <unistd.h>
# include <vector>
# include <map>
# include <dirent.h>
# include <sys/wait.h>
# include <algorithm>

# include "libwebserv.hpp"

class Lexer;

class CGI {

	private:
		// Functions
		CGI(void);
		
		bool runCGI(std::string request);
		bool validPath();
		bool validExtension();
		void runScript();
		std::string getExtension();
		void parseFileFromRequest(std::string request);
		void eraseNewline();
		void setExtensions();
		char ** chooseExtension();

		// Variables
		std::string method;
		std::string filename;
		std::string filePath;
		std::string extension;
		std::string runner;
		std::string error;
		std::vector<std::string> allowedExts;


	public:
		CGI(std::string request);
		~CGI(void) {};

		std::string response;

};

	class CGIException : public std::exception {
		public:
			CGIException(const std::string& message) : message(message) {};

			virtual const char* what() const throw() {
				return message.c_str();
			};
		private:
			const std::string& message;
	};

#endif // CGI_HPP