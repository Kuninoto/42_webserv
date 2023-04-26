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
// # include "utils.hpp"

class Lexer;

class CGI {

	private:
		CGI(void);
		std::map<std::string, std::vector<std::string> > routeMethods;
		std::map<std::string, std::string> routePaths;
		std::string response;

		//
		std::string indexFile;
		bool directoryListingEnabled;
		std::string root;
		std::string methods;
		std::map<std::string, std::string> config;

	public:
		CGI(Server parameters);
		~CGI(void) {};

		bool isRegularFile(const char *path);
		void dirListing(std::string requestedPath);
		void handleDirectoryListing(const std::string& path);
		bool isDirectory(const char *path);

};

#endif // CGI_HPP