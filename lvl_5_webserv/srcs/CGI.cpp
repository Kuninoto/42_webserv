#include "CGI.hpp"

#include <unistd.h>

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;

CGI::CGI(void) {
    runCGI();

    cout << "Method: " << envVars["REQUEST_METHOD"] << endl;
    cout << "Filename: " << envVars["SCRIPT_NAME"] << endl;
    cout << "Ext: " << extension << endl;
    cout << "CGI File path: " << filePath << endl;
    cout << "File is valid: "
         << "yes" << endl;
    cout << "Extension is valid: "
         << "yes" << endl;
}

CGI::~CGI(void) {
    remove(".output");
    // delete[] this->args;
}

void CGI::runCGI(void) {
    this->getEnvVars();
    this->setExtension();
    if (access(filePath.c_str(), F_OK) != 0)
        throw CGIException("Invalid CGI file");
    this->runScript();
}

/**
 * @brief gets the environment variables and checks if all obligatory ones are present
 *
 * @return true if everything is good
 * @return false if an obligatory variable is missing
 */
void CGI::getEnvVars(void) {
    envVars["REQUEST_METHOD"] = getenv("REQUEST_METHOD");
    envVars["PATH_INFO"] = getenv("PATH_INFO");
    envVars["SCRIPT_NAME"] = getenv("SCRIPT_NAME");
    envVars["QUERY_STRING"] = getenv("QUERY_STRING");
    envVars["CONTENT_LENGTH"] = getenv("CONTENT_LENGTH");
    envVars["CONTENT_TYPE"] = getenv("CONTENT_TYPE");

    this->checkVars(envVars["REQUEST_METHOD"]);

    if (!envVars["QUERY_STRING"].empty())
        this->parseQueryString();
    this->setExtension();
    this->filePath = envVars["PATH_INFO"] + envVars["SCRIPT_NAME"];
}

/**
 * @brief checks if all variables are present for the request type
 *
 * @param method either GET, POST or DELETE
 * @return true
 * @return false
 */
void CGI::checkVars(std::string method) {
    if (envVars["REQUEST_METHOD"].empty())
        throw CGIException("REQUEST_METHOD variable missing");
    if (envVars["REQUEST_METHOD"] != "GET" && envVars["REQUEST_METHOD"] != "POST")
        throw CGIException("Bad variable REQUEST_METHOD");
    if (envVars["PATH_INFO"].empty())
        throw CGIException("PATH_INFO variable missing");

    if (envVars["SCRIPT_NAME"].empty())
        throw CGIException("SCRIPT_NAME variable missing");

    if (method == "POST") {
        if (envVars["QUERY_STRING"].empty())
            throw CGIException("QUERY_STRING variable missing");
        if (envVars["CONTENT_LENGTH"].empty())
            throw CGIException("CONTENT_LENGTH variable missing");
        if (envVars["CONTENT_TYPE"].empty())
            throw CGIException("CONTENT_TYPE variable missing");
    }
}

void CGI::setExtension(void) {
    size_t pos = envVars["SCRIPT_NAME"].find_last_of('.');
    if (pos == std::string::npos)
        throw CGIException("Target CGI has no extension");
    this->extension = envVars["SCRIPT_NAME"].substr(pos + 1);
    if (!(extension == "py" || extension == "php" || extension == "sh"))
        throw CGIException("Unknown CGI extension");
}

/**
 * @brief run the script and save the output into file
 *
 */
void CGI::runScript(void) {
    pid_t pid;

    int output_fd = open(".output", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (output_fd == -1)
        throw CGIException("open() failed");

    pid = fork();
    if (pid == -1)
        throw CGIException("fork() failed");

    // child
    if (pid == 0) {
        dup2(output_fd, STDOUT_FILENO);
        close(output_fd);
        createArgs();
        execve(runner.c_str(), args, NULL);
        // only gets here if execve fails
        throw CGIException("execve() failed");
    } else {
        int status;

        while (waitpid(pid, &status, 0) == -1)
            ;
        close(output_fd);
    }
}

/**
 * @brief chooses the right way to run the script
 *
 */
void CGI::createArgs(void) {
    int i = 2;

    if (extension == "py") {
        runner = "/usr/bin/python3";
        args = new char *[3 + params.size()];
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
        args = new char *[3 + params.size()];
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
        args = new char *[3 + params.size()];
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
 * @brief parses the query string so that it can be passed as arguments to the script
 *
 */
void CGI::parseQueryString(void) {
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