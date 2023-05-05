#include "CGI.hpp"

#include <unistd.h>

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;

CGI::CGI(std::string request) {
    if (!runCGI(request))
        throw(CGIException(error));

    //! Below is for debbuging, delete when done
    cout << "Method: " << method << endl;
    cout << "Filename: " << filename << endl;
    cout << "Ext: " << extension << endl;
    cout << "File path: " << filePath << endl;
    cout << "File is valid: "
         << "yes" << endl;
    cout << "Extension is valid: "
         << "yes" << endl;
}

/**
 * @brief function to run the CGI process
 *
 * @param request HTTP request header
 * @return true if operation was a success
 * @return false if something went wrong
 */
bool CGI::runCGI(const std::string &request) {
    this->parseFileFromRequest(request);
    this->filePath = getenv("PATH_INFO") + ("/" + filename);
    this->extension = this->getExtension();
    if (!this->validExtension()) {
        this->error = "Invalid file type";
        return false;
    }
    if (!this->validPath()) {
        this->error = "Invalid file";
        return false;
    }

    try {
        this->runScript();
    } catch (const std::exception &e) {
        this->error = e.what();
        return false;
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
 * @param ext file extension
 * @return true if valid
 * @return false if not valid
 */
bool CGI::validExtension(void) {
    if (extension == "py" || extension == "php" || extension == "sh")
        return true;
    return false;
}

/**
 * @brief Returns the extension of the file stored in "filename" variable
 *
 * @return std::string
 */
std::string CGI::getExtension(void) {
    std::string ext;

    int pos = this->filename.find_last_of('.');
    // if (pos == std::string::npos)
    //	error
    ext = this->filename.substr(pos + 1);
    return ext;
}

/**
 * @brief Parses the HTTP request and stores information into variables
 *
 * @param request is the HTTP request header
 */
void CGI::parseFileFromRequest(std::string request) {
    std::stringstream ss(request);

    // Read the HTTP request and save the first and second word
    getline(ss, method, ' ');
    getline(ss, filename, ' ');
}

/**
 * @brief run the script and save the output into response
 *
 */
void CGI::runScript(void) {
    int pipefd[2];
    char buffer[1024];
    pid_t pid;

    if (pipe(pipefd) == -1)
        throw(CGIException("pipe() failed"));

    pid = fork();
    if (pid == -1)
        throw(CGIException("fork() failed"));
    // child
    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        char **args = chooseExtension();
        execve(runner.c_str(), args, NULL);
        // only gets here if execve fails
        delete args;
        exit(127);
    } else {
        close(pipefd[1]);
        int status;
        while (waitpid(pid, &status, 0) == -1)
            ;
        while (read(pipefd[0], &buffer, 1023) != 0) {
            response += buffer;
            memset(&buffer, '\0', 1024);
        }
        close(pipefd[0]);
    }
}

/**
 * @brief chooses the right way to run the script
 *
 * @return char* are the arguments for execve
 */
char **CGI::chooseExtension(void) {
    char **args;

    if (extension == "py") {
        runner = "/usr/bin/python3";
        args = new char *[3];
        args[0] = (char *)"python3";
        args[1] = (char *)filePath.c_str();
        args[2] = NULL;
    }

    if (extension == "php") {
        runner = "/usr/bin/php";
        args = new char *[3];
        args[0] = (char *)"php";
        args[1] = (char *)filePath.c_str();
        args[2] = NULL;
    }

    if (extension == "sh") {
        runner = "/usr/bin/bash";
        args = new char *[3];
        args[0] = (char *)"bash";
        args[1] = (char *)filePath.c_str();
        args[2] = NULL;
    }

    return args;
}