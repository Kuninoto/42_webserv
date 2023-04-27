#include "CGI.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>

using std::cout;
using std::cerr;
using std::endl;

CGI::CGI(std::string request)
{
	if (!runCGI(request))
		throw(CGIException(error));

	//! Below is for debbuging, delete when done
	cout << "Method: " << method << endl;
	cout << "Filename: " << filename << endl;
	cout << "Ext: " << extension << endl;
	cout << "File path: " << filePath << endl;
	cout << "File is valid: " << validPath() << endl;
	cout << "Extension is valid: " << validExtension() << endl;
}

/**
 * @brief function to run the CGI process
 * 
 * @param request HTTP request header 
 * @return true if operation was a success
 * @return false if something went wrong
 */
bool CGI::runCGI(std::string request)
{
	parseFileFromRequest(request);
	filePath = getenv("PATH_INFO") + filename;
	extension = getExtension();
	setExtensions();
	if (!validPath())
	{
		error = "Error: Invalid file";
		return false;
	}
	if (!validExtension())
	{
		error = "Error: Invalid file type";
		return false;
	}

	try
	{
		runScript();
	}
	catch(const std::exception& e)
	{
		error = e.what();
		return false;
	}
	eraseNewline();

	return true;
}

/**
 * @brief checks if filePath is a valid path
 * 
 * @return true if valid
 * @return false if not valid
 */
bool	CGI::validPath()
{
	struct stat buffer;
	return (stat(filePath.c_str(), &buffer) == 0);
}

/**
 * @brief Checks if file extension is valid
 * 
 * @param ext file extension
 * @return true if valid
 * @return false if not valid
 */
bool	CGI::validExtension()
{
	std::vector<std::string>::iterator it = find(allowedExts.begin(), allowedExts.end(), extension);

	if (it != allowedExts.end() && (strcmp(it->c_str(), extension.c_str()) == 0))
		return true;
	else
		return false;
}

/**
 * @brief Returns the extension of the file stored in "filename" variable
 * 
 * @return std::string 
 */
std::string	CGI::getExtension()
{
	std::string ext;

	int pos = filename.find_last_of('.');
	ext = filename.substr(pos + 1);

	return ext;
}

/**
 * @brief Parses the HTTP request and stores information into variables
 * 
 * @param request is the HTTP request header
 */
void	CGI::parseFileFromRequest(std::string request)
{
	std::stringstream ss(request);

	// Read the HTTP request and save the first and second word
	getline(ss, method, ' ');
	getline(ss, filename, ' ');
}

/**
 * @brief run the script and save the output into response
 * 
 */
void	CGI::runScript()
{
	int pipefd[2];
	char buffer[128];
	pid_t pid;

	if (pipe(pipefd) == -1)
		throw(CGIException("Error: Pipe failed"));

	pid = fork();

	if (pid == -1)
		throw(CGIException("Error: Fork failed"));
	else if (pid == 0) {
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		dup2(pipefd[1], STDERR_FILENO);
		close(pipefd[1]);
		char **args = chooseExtension();
		execve(runner.c_str(), (char* const*)args, NULL);
		delete args;
		_exit(127);
	}
	else {
		close(pipefd[1]);
		int status;
		while (waitpid(pid, &status, 0) == -1);
		while (read(pipefd[0], buffer, sizeof(buffer)) != 0) {
			response += buffer;
		}
		close(pipefd[0]);
	}
}

/**
 * @brief erases the last '\n' in the string if it exists
 * 
 */
void	CGI::eraseNewline()
{
	if (response.at(response.length() - 1) == '\n')
		response.erase(response.find_last_of('\n'), 1);
}

/**
 * @brief defines allowed/recognized extensions for the cgi
 * 
 */
void CGI::setExtensions()
{
	allowedExts.push_back("php");
	allowedExts.push_back("py");
	allowedExts.push_back("sh");
}

/**
 * @brief chooses the right way to run the script
 * 
 * @return char* are the arguments for execve
 */
char **	CGI::chooseExtension()
{
	char** args;

	if (extension == "py") {
		runner = "/usr/bin/python3";
		args = new char*[3];
		args[0] = (char *)"python3";
		args[1] = (char *)filePath.c_str();
		args[2] = NULL;
	}
	
	if (extension == "php") {
		runner = "/usr/bin/php";
		args = new char*[3];
		args[0] = (char *)"php";
		args[1] = (char *)filePath.c_str();
		args[2] = NULL;
	}
	
	if (extension == "sh") {
		runner = "/usr/bin/bash";
		args = new char*[3];
		args[0] = (char *)"bash";
		args[1] = (char *)filePath.c_str();
		args[2] = NULL;
	}

	return args;
}