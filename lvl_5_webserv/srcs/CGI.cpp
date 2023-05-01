#include "CGI.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>

using std::cout;
using std::endl;

CGI::CGI()
{
	if (!runCGI())
		throw (CGIException(error));

	//! Below is for debbuging, delete when done
	cout << "Method: " << envVars["REQUEST_METHOD"] << endl;
	cout << "Filename: " << envVars["SCRIPT_NAME"] << endl;
	cout << "Ext: " << extension << endl;
	cout << "File path: " << filePath << endl;
	cout << "File is valid: " << "yes" << endl;
	cout << "Extension is valid: " << "yes" << endl;
}

CGI::~CGI(void) {
	// if (args[0])
	// 	delete args;
}

/**
 * @brief function to run the CGI process
 *
 * @param request HTTP request header
 * @return true if operation was a success
 * @return false if something went wrong
 */
bool CGI::runCGI()
{
	// Get environment variables
	if (!this->getEnvVars())
		return false;

	if (!this->validExtension())
		return(this->retError("Invalid file type"));

	if (!this->validPath())
		return(this->retError("Invalid file"));

	try {
		this->runScript();
	}
	catch (const std::exception& e) {
		this->error = e.what();
		return false;
	}
	return true;
}

/**
 * @brief gets the environment variables and checks if all obligatory ones are present
 *
 * @return true if everything is good
 * @return false if an obligatory variable is missing
 */
bool CGI::getEnvVars()
{
	envVars["REQUEST_METHOD"] = getenv("REQUEST_METHOD");
	envVars["PATH_INFO"] = getenv("PATH_INFO");
	envVars["SCRIPT_NAME"] = getenv("SCRIPT_NAME");
	envVars["QUERY_STRING"] = getenv("QUERY_STRING");
	envVars["CONTENT_LENGTH"] = getenv("CONTENT_LENGTH");
	envVars["CONTENT_TYPE"] = getenv("CONTENT_TYPE");

	if (!checkVars(envVars["REQUEST_METHOD"]))
		return false;

	if (!envVars["QUERY_STRING"].empty())
		parseQueryString();
	if (!getExtension())
		return(retError("The file has no extension"));
	filePath = envVars["PATH_INFO"] + ("/" + envVars["SCRIPT_NAME"]);
	return true;
}

/**
 * @brief checks if all variables are present for the request type
 *
 * @param method either GET, POST or DELETE
 * @return true
 * @return false
 */
bool CGI::checkVars(std::string method)
{
	if (envVars["REQUEST_METHOD"].empty())
		return(retError("REQUEST_METHOD variable missing"));
	if (envVars["REQUEST_METHOD"] != "GET" && envVars["REQUEST_METHOD"] != "POST")
	{
		return(retError("Bad variable REQUEST_METHOD"));
	}

	if (envVars["PATH_INFO"].empty())
		return(retError("PATH_INFO variable missing"));

	if (envVars["SCRIPT_NAME"].empty())
		return(retError("SCRIPT_NAME variable missing"));

	if (method == "POST") {
		if (envVars["QUERY_STRING"].empty())
			return(retError("QUERY_STRING variable missing"));
		if (envVars["CONTENT_LENGTH"].empty())
			return(retError("CONTENT_LENGTH variable missing"));
		if (envVars["CONTENT_TYPE"].empty())
			return(retError("CONTENT_TYPE variable missing"));
	}

	return true;
}

/**
 * @brief checks if filePath is a valid path
 *
 * @return true if valid
 * @return false if not valid
 */
bool CGI::validPath(void) {
	return (access(filePath.c_str(), F_OK) == 0);
}

/**
 * @brief Checks if file extension is valid
 *
 * @return true if valid
 * @return false if not valid
 */
bool CGI::validExtension(void)
{
	if (extension == "py" || extension == "php" || extension == "sh")
		return true;
	return false;
}

/**
 * @brief Returns the extension of the file stored in "filename" variable
 *
 * @return std::string returns a string with the extension
 */
bool	CGI::getExtension(void)
{
	size_t pos = envVars["SCRIPT_NAME"].find_last_of('.');
	if (pos == std::string::npos)
		return false;
	extension = envVars["SCRIPT_NAME"].substr(pos + 1);
	return true;
}

/**
 * @brief run the script and save the output into response
 *
 */
void CGI::runScript(void)
{
	int pipefd[2];
	char buffer[1024];
	pid_t pid;
	if(!deleteFile())
		throw(CGIException("File not deleted"));
	std::ofstream output(".output");
	if (pipe(pipefd) == -1)
		throw (CGIException("pipe() failed"));

	pid = fork();
	if (pid == -1)
		throw (CGIException("fork() failed"));
	// child
	if (pid == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		creatArgs();
		execve(runner.c_str(), args, NULL);
		// only gets here if execve fails
		throw (CGIException("execve() failed"));
	}
	else
	{
		close(pipefd[1]);
		int status;
		while (waitpid(pid, &status, 0) == -1);
	
		// Read from pipe and write to file
		while (read(pipefd[0], &buffer, 1023) != 0)
		{
			buffer[1023] = '\0';
			output << buffer;
			cout << "buffer \"" << buffer << "\"" << endl;
			memset(&buffer, '\0', 1024);
		}
		output.flush(); // Flush the output to the file
		close(pipefd[0]);
	}
}

bool CGI::deleteFile() {
	if (std::FILE* file = std::fopen(".output", "r")) {
		std::fclose(file);
		// delete file
		if (std::remove(".output") != 0)
			return false;
		cout << "file deleted" << endl;
	}
	return true;
}

/**
 * @brief chooses the right way to run the script
 *
 */
void	CGI::creatArgs(void)
{
	int i = 2;

	if (extension == "py") {
		runner = "/usr/bin/python3";
		args = new char*[3 + params.size()];
		args[0] = (char *)"python3";
		args[1] = (char *)filePath.c_str();
		for (size_t param = 0; param < params.size(); param++) {
			args[i] = (char *)params[param].c_str();
			i++;
		}
		args[i] = NULL;
	}

	if (extension == "php") {
		runner = "/usr/bin/php";
		args = new char*[3 + params.size()];
		args[0] = (char *)"php";
		args[1] = (char *)filePath.c_str();
		for (size_t param = 0; param < params.size(); param++) {
			args[i] = (char *)params[param].c_str();
			i++;
		}
		args[i] = NULL;
	}

	if (extension == "sh") {
		runner = "/usr/bin/bash";
		args = new char*[3 + params.size()];
		args[0] = (char *)"bash";
		args[1] = (char *)filePath.c_str();
		for (size_t param = 0; param < params.size(); param++) {
			args[i] = (char *)params[param].c_str();
			i++;
		}
		args[i] = NULL;
	}
}

/**
 * @brief sets "error" to the error that should be thrown and returns false
 *
 * @param error error message to be thrown
 * @return always false
 */
bool	CGI::retError(std::string message)
{
	this->error = message;
	return false;
}

/**
 * @brief parses the query string so that it can be passed as arguments to the script
 *
 */
void CGI::parseQueryString()
{
	std::string value, param;
	size_t pos, equal;
	std::string qs(envVars["QUERY_STRING"]);

	// cout << "Query string = " << qs << endl;

	while (!qs.empty()) {
		pos = qs.find("&");

		if (pos != std::string::npos) {
			param = qs.substr(0, pos);
		} else {
			param = qs;
			qs.clear();
		}

		equal = param.find("=");
		if (equal != std::string::npos) {
			value = param.substr(equal + 1, pos);
			qs.erase(0, pos + 1);
		} else {
			value = param;
		}

		for (size_t i = 0; i < value.size(); i++) {
			if (value[i] == '+')
				value[i] = ' ';
		}

		params.push_back(value);
	}

	// Print the parameters and their values
	// for (size_t i = 0; i < params.size(); i++) {
	// 	cout << "vector at " << i << " = \"" << params.at(i) << "\"" << endl;
	// }
}