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
		CGI(void);
		~CGI(void) {delete args;};

		std::string response;

	private:
		
		bool runCGI();
		bool validPath(void);
		bool validExtension(void);
		void runScript(void);
		bool getExtension(void);
		void creatArgs(void);
		bool retError(std::string message);
		void parseQueryString();
		bool getEnvVars();

		char **args;
		std::vector<std::string> params;
		std::map<std::string, std::string> envVars;
		std::string method;
		std::string filePath;
		std::string extension;
		std::string runner;
		std::string error;
};

	class CGIException : public std::exception {
		public:
			CGIException(const std::string error) throw() {message = new std::string(error);};
			virtual ~CGIException() throw() {delete message;}

			virtual const char* what() const throw() {
				return message->c_str();
			};
		private:
			const std::string *message;
	};

#endif // CGI_HPP