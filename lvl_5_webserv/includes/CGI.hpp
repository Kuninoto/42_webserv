#ifndef CGI_HPP
#define CGI_HPP

#include <sys/stat.h>
#include <cstdlib>
#include <stdint.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <map>
#include <dirent.h>
#include <sys/wait.h>
#include <algorithm>

#include "libwebserv.hpp"

class Lexer;

class CGI {
	public:
		CGI(std::string request);
		~CGI(void) {};

		std::string response;

	private:
		CGI(void);
		
		bool runCGI(const std::string& request);
		bool validPath(void);
		bool validExtension(void);
		void runScript(void);
		std::string getExtension(void);
		void parseFileFromRequest(std::string request);
		char **chooseExtension(void);

		std::string method;
		std::string filename;
		std::string filePath;
		std::string extension;
		std::string runner;
		std::string error;
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