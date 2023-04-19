#include "../includes/CGI.hpp"

CGI::CGI() {
	routeMethods["/general"] = {"GET", "POST", "DELETE"};
	routeMethods["/pages"] = {"GET", "POST"};

	routePaths["/pages"] = "/home/Flirt/Desktop/Projects_42/Webserver/lvl_5_webserv/pages/";
	routePaths["/."] = "/home/Flirt/Desktop/Projects_42/Webserver/lvl_5_webserv/";
}

std::string formatTime(time_t mod_time)
{
	char buf[80];
	std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&mod_time));
	return std::string(buf);
}


bool CGI::isRegularFile(const char *path)
{
	struct stat st;
	if (stat(path, &st) != 0) {
		return false;
	}
	return S_ISREG(st.st_mode);
}


bool CGI::isDirectory(const std::string& path)
{
	if (access(path.c_str(), F_OK) != 0)
	{
		return false;  // path does not exist
	}

	struct stat fileStat;
	if (stat(path.c_str(), &fileStat) == 0)
	{
		return S_ISDIR(fileStat.st_mode);
	}
	return false;
}

void CGI::handle_directory_listing(TcpConnection& connection, const std::string& path)
{
	// Check if directory listing is allowed
	if (!directoryListingEnabled) {
		//! throw(HTTP_STATUS_FORBIDDEN);
		return;
	}

	// Check if path is a directory
	if (!isDirectory(path)) {
		//! throw(HTTP_STATUS_NOT_FOUND);
		return;
	}

	// Generate directory listing HTML
	std::string html = "<html><head><title>Index of " + path + "</title></head><body>\n";
	html += "<h1>Index of " + path + "</h1>\n";
	html += "<table>\n";
	html += "<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\n";

	DIR*			dir = opendir(path.c_str());
	struct dirent*	entry;

	while ((entry = readdir(dir)) != NULL)
	{
		string name = entry->d_name;
		string full_path = path + "/" + name;
		struct stat st;

		if (stat(full_path.c_str(), &st) == -1)
		{
			continue;
		}

		string			modified = formatTime(st.st_mtime);
		uintmax_t		size = isRegularFile(full_path.c_str()) ? st.st_size : 0;

		html += "<tr><td><a href=\"" + name + "\">" + name + "</a></td><td>" + modified + "</td><td>" + std::to_string(size) + "</td></tr>\n";

		closedir(dir);

		html += "</table>\n";
		html += "</body></html>\n";

		// Send response
		// TODO create response class
		// HttpResponse response;
		// response.status = HTTP_STATUS_OK;
		// response.content_type = "text/html";
		// response.content = html;
		// TODO create general response delivery function
		// sendResponse(connection, response);
	}
}

void	CGI::dirListing(string requestedPath)
{
	directoryListingEnabled = true;

	string requestedPath = "/home/Flirt/Desktop/Projects_42/Webserver/lvl_5_webserv";
	struct stat fileStat;
	if (stat(requestedPath.c_str(), &fileStat) < 0) {
		// file does not exist
		cout << "File does not exist" << endl;
		// ! throw
	}
	if (S_ISDIR(fileStat.st_mode))
	{
		// requested path is a directory
		if (directoryListingEnabled)
		{
			// send directory listing HTML response
			string defaultFilePath = requestedPath + "/" + defaultFileName;
			if (access(defaultFilePath.c_str(), F_OK) == 0)
			{
			// requested path is a directory
				// default file exists, serve it
				// ...
			}
			else
				cout << "couldn't find " << defaultFilePath << endl;
				//! throw
		}
		else
		{
			// send 403 Forbidden response
			string response = "HTTP/1.1 403 Forbidden\r\n\r\n";
		}
	}
	else
	{
		// requested path is a file
		// ...
	}
}

int main()
{
	string redirectUrl = "http://localhost/new";

	CGI test;

	test.dirListing("/home/Flirt/Desktop/Projects_42/Webserver/lvl_5_webserv");

	//* testing response
	string response;
	response += "HTTP/1.1 301 Moved Permanently\r\n";
	response += "Location: ";
	response += redirectUrl;
	response += "\r\n\r\n";

	cout << response << endl;

}