#ifndef CGI_HPP
# define CGI_HPP

# include <sys/stat.h>
# include <cstdlib>
# include <string>
# include <unistd.h>
# include <iostream>
# include <arpa/inet.h>
# include <poll.h>
# include <memory.h>
# include <vector>
# include <map>
# include <dirent.h>
# include <ctime>

# include "TcpConnection.hpp"
# include "utils.hpp"


using std::cout;
using std::endl;
using std::string;
using std::map;
using std::vector;

class CGI {

	private:
		map<string, vector<string>>	routeMethods;
		map<string, string>			routePaths;
		bool 						directoryListingEnabled;
		string 						defaultFileName = "index.html";


	public:
		CGI() {};
		~CGI() {};

		std::string	formatTime(time_t mod_time);
		bool		isRegularFile(const char *path);
		void		dirListing(string requestedPath);
		void		handle_directory_listing(TcpConnection& connection, const std::string& path);
		bool		isDirectory(const std::string& path);

};

#endif