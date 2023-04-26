#include "CGI.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1000

using std::cout;
using std::cerr;
using std::endl;

CGI::CGI(Server parameters)
{
	indexFile = parameters.getIndex();
	root = parameters.getRoot();

	//TODO: ask Nuno if there is a getListing

	std::map<std::string, location_t>::iterator it = parameters.locations.find("")
	if (parameters.locations.find() == "on")
	directoryListingEnabled = true;
	// else
	// 	directoryListingEnabled = false;

	cout << "Index: " << indexFile << endl;
	cout << "Root: " << root << endl;
	cout << "Dir listing: " << directoryListingEnabled << endl;

}

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

void CGI::handleDirectoryListing(const std::string& path)
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
		uint64_t	toConvSize = isRegularFile(full_path.c_str()) ? st.st_size : 0;

		std::stringstream ss;
		ss << toConvSize;
		std::string size = ss.str();


		html += "<tr><td><a href=\"" + name + "\">" + name + "</a></td><td>" + modified + "</td><td>" + size + "</td></tr>\n";
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
			std::string response = "HTTP/1.1 403 Forbidden\r\n\r\n";
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

// void execute_cgi(int client_fd, const char* cgi_path, const char* path_info, const char* method, const char* query_string, const char* content_type, const char* content_length)
// {
// 	int to_cgi[2], from_cgi[2];
// 	pid_t pid;

// 	// Create pipe for communication with CGI process
// 	if (pipe(to_cgi) < 0 || pipe(from_cgi) < 0)
// 	{
// 		perror("pipe");
// 		exit(EXIT_FAILURE);
// 	}

// 	// Fork a new process for the CGI program
// 	pid = fork();
// 	if (pid < 0)
// 	{
// 		perror("fork");
// 		exit(EXIT_FAILURE);
// 	}
// 	else if (pid == 0)
// 	{
// 		// Child process - redirect stdin, stdout, stderr to pipes
// 		dup2(to_cgi[0], STDIN_FILENO);
// 		dup2(from_cgi[1], STDOUT_FILENO);
// 		dup2(from_cgi[1], STDERR_FILENO);

// 		// Close unused ends of pipes
// 		close(to_cgi[1]);
// 		close(from_cgi[0]);

// 		// Set environment variables for CGI program
// 		setenv("REQUEST_METHOD", method, 1);
// 		setenv("SCRIPT_FILENAME", cgi_path, 1);
// 		setenv("PATH_INFO", path_info, 1);
// 		setenv("QUERY_STRING", query_string, 1);
// 		setenv("CONTENT_TYPE", content_type, 1);
// 		setenv("CONTENT_LENGTH", content_length, 1);

// 		// Execute the CGI program
// 		execl(cgi_path, cgi_path, NULL);

// 		// Should never reach here unless execl fails
// 		perror("execl");
// 		exit(EXIT_FAILURE);
// 	}
// 	else
// 	{
// 		// Parent process - close unused ends of pipes
// 		close(to_cgi[0]);
// 		close(from_cgi[1]);

// 		// If the request is POST, write the content to the pipe to the CGI process
// 		if (strcmp(method, "POST") == 0)
// 		{
// 			char buffer[MAX_BUFFER_SIZE];
// 			int bytes_read;

// 			// Read content from client and write to pipe
// 			while ((bytes_read = read(client_fd, buffer, MAX_BUFFER_SIZE)) > 0)
// 				write(to_cgi[1], buffer, bytes_read);
// 		}

// 		// Read output from CGI process and send to client
// 		char buffer[MAX_BUFFER_SIZE];
// 		int bytes_read;

// 		while ((bytes_read = read(from_cgi[0], buffer, MAX_BUFFER_SIZE)) > 0)
// 			write(client_fd, buffer, bytes_read);

// 		// Close remaining ends of pipes and wait for child process to finish
// 		close(to_cgi[1]);
// 		close(from_cgi[0]);

// 		int status;
// 		waitpid(pid, &status, 0);

// 		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
// 		{
// 			// CGI program returned an error status
// 			fprintf(stderr, "CGI program returned error status %d\n", WEXITSTATUS(status));
// 		}
// 	}
// }

// bool handleCgi(int client_fd, const char* cgi_path, const char* path_info, const char* method, const char* query_string, const char* content_type, const char* content_length)
// {
// 	// Get file extension
// 	std::string pathInfo = std::getenv("PATH_INFO");
// 	std::string fileExt = pathInfo.substr(pathInfo.rfind(".") + 1);

// 	if (!pathInfo.c_str())
// 	{
// 		cout << "MY_ENV_VAR is not set" << endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	// Check if file extension is valid for CGI execution
// 	std::unordered_set<std::string> cgi_exts = {"php", "py", "pl"};
// 	if (cgi_exts.find(fileExt) == cgi_exts.end()) {
// 		cout << "File extension not supported" << endl;
// 		//! throw
// 		return false;
// 	}

// 	// Create pipe for communication between parent and child process
// 	int pipefd[2];
// 	if (pipe(pipefd) == -1) {
// 		response += "500\n";
// 		response += "Internal Server Error\n";
// 		return true;
// 	}

// 	// Fork a child process to execute the CGI script
// 	pid_t pid = fork();
// 	if (pid == -1) {
// 		response += "500\n";
// 		response += "Internal Server Error\n";
// 		return true;
// 	}

// 	std::string completePath = root;
// 	completePath += path_info;
// 	// Child process
// 	if (pid == 0) {
// 		// Set environment variables for CGI script
// 		setenv("REQUEST_METHOD", method, 1);
// 		setenv("SCRIPT_FILENAME", completePath.c_str(), 1);
// 		setenv("QUERY_STRING", query_string, 1);
// 		setenv("CONTENT_TYPE", content_type, 1);
// 		setenv("CONTENT_LENGTH", content_length, 1);
// 		setenv("PATH_INFO", path_info, 1);

// 		// Close read end of pipe
// 		close(pipefd[0]);

// 		// Redirect STDOUT to write end of pipe
// 		dup2(pipefd[1], STDOUT_FILENO);

// 		// Execute CGI script
// 		execl("/usr/bin/php-cgi", "php-cgi", NULL);

// 		// If execl fails, write to pipe to signal error
// 		write(pipefd[1], "ERROR", 5);

// 		// Exit child process
// 		exit(EXIT_FAILURE);
// 	}
// 	// Parent process
// 	else {
// 		// Close write end of pipe
// 		close(pipefd[1]);

// 		// Wait for child process to finish executing
// 		int status;
// 		waitpid(pid, &status, 0);

// 		// Read output from child process
// 		std::stringstream ss;
// 		char buffer[1024];
// 		int bytes_read;
// 		while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
// 			ss.write(buffer, bytes_read);
// 		}

// 		// Close read end of pipe
// 		close(pipefd[0]);

// 		// Set response body to output from CGI script
// 		response += ss.str();

// 		// Set content type based on file extension
// 		if (fileExt == "php") {
// 			response.set_header("Content-Type", "text/html");
// 		} else if (fileExt == "py") {
// 			response.set_header("Content-Type", "text/plain");
// 		} else if (fileExt == "pl") {
// 			response.set_header("Content-Type", "text/html");
// 		}

// 		return true;
// 	}
// }
