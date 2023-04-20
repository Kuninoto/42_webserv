#include "CGI.hpp"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

bool CGI::isRegularFile(const char *path)
{
	struct stat st;
	if (stat(path, &st) != 0) {
		return false;
	}
	return S_ISREG(st.st_mode);
}


bool CGI::isDirectory(const char *path)
{
	if (access(path, F_OK) != 0) {
		return false;  // path does not exist
	}

	struct stat fileStat;
	if (stat(path, &fileStat) == 0) {
		return S_ISDIR(fileStat.st_mode);
	}
	return false;
}

void CGI::handleDirectoryListing(const std::string& path)c
{
	// Generate directory listing HTML
	std::string html = "<html><head><title>Index of " + path + "</title></head><body>\n";
	html += "<h1>Index of " + path + "</h1>\n";
	html += "<table>\n";
	html += "<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\n";

	DIR* dir = opendir(path.c_str());
	struct dirent* entry;

	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		std::string full_path = path + "/" + name;
		struct stat st;

		if (stat(full_path.c_str(), &st) == -1)
		{
			continue;
		}

		std::string modified = getTimeStamp();
		uintmax_t	size = isRegularFile(full_path.c_str()) ? st.st_size : 0;

		html += "<tr><td><a href=\"" + name + "\">" + name + "</a></td><td>" + modified + "</td><td>" + std::to_string(size) + "</td></tr>\n";
		html += "</table>\n";
		html += "</body></html>\n";

		closedir(dir);

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

void	CGI::dirListing(std::string requestedPath)
{
	directoryListingEnabled = true;

	// string requestedPath = "/home/Flirt/Desktop/Projects_42/Webserver/lvl_5_webserv";
	struct stat fileStat;
	if (stat(requestedPath.c_str(), &fileStat) < 0) {
		// file does not exist
		cout << "File does not exist" << endl;
		// ! throw
	}

	if (isDirectory(requestedPath.c_str()))
	{
		// requested path is a directory
		if (directoryListingEnabled)
			// send directory listing HTML response
			handleDirectoryListing(requestedPath);
		else
			// send 403 Forbidden response
			string response = "HTTP/1.1 403 Forbidden\r\n\r\n";
	}
	else if (isRegularFile(requestedPath.c_str()))
	{
		// requested path is a file
		cout << "It's a file! :)" << endl;
		// ...
	}
	else
	{
		// Check if path is a directory
		if (!isDirectory(requestedPath.c_str())) {
			//! throw(HTTP_STATUS_NOT_FOUND);
			return;
		}
		// requested path doesn't exist
		cout << "requested path doesn't exist" << endl;
		//! throw
	}
}