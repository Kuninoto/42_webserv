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

# include "libwebserv.hpp"

class Lexer;

class CGI {

	private:
		// Functions
		CGI(void);
		
		void parseFileFromRequest(std::string request);
		std::string getExtension();

		// Variables
		std::string filename;

	public:
		CGI(std::string request);
		~CGI(void) {};



};

#endif // CGI_HPP