#ifndef CGI_HPP
# define CGI_HPP

# include <sys/stat.h>
# include <cstdlib>
# include <cstdint>
# include <string>
# include <unistd.h>
# include <vector>
# include <map>
# include <dirent.h>
# include <ctime>

# include "TcpConnection.hpp"
# include "utils.hpp"

class CGI {

	private:
		std::map<std::string, std::vector<std::string> >	routeMethods;
		std::map<std::string, std::string>			routePaths;
		bool 						directoryListingEnabled;
		std::string 						defaultFileName = "index.html";


	public:
		CGI() {};
		~CGI() {};

		std::string	formatTime(time_t mod_time);
		bool		isRegularFile(const char *path);
		void		dirListing(std::string requestedPath);
		void		handle_directory_listing(TcpConnection& connection, const std::string& path);
		bool		isDirectory(const std::string& path);

};

#endif