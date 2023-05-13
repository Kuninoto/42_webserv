#include "CGI.hpp"

#include <unistd.h>

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;

CGI::CGI(const std::string& cgi_path, const std::string& cgi_ext)
    : cgi_path(cgi_path), cgi_ext(cgi_ext) {
    cout << "RUNNING CGI" << endl;
    cout << "CGI PATH: " << this->cgi_path << endl;
    cout << "CGI Ext: " << this->cgi_ext << endl;

    std::string ext = cgi_path.substr(cgi_path.find('.'));
    if (ext != cgi_ext)
        throw CGIException("Invalid extension \"" + ext + "\" expected \"" + cgi_ext + "\"");
    this->runCGI();
}

CGI::~CGI(void) {
    remove(".cgi_output");
    for (size_t i = 0; this->args[i]; i += 1)
        free(this->args[i]);
    delete[] this->args;
}

void CGI::runCGI(void) {
    this->getEnvVars();
    // this->checkVars();
    if (access(this->cgi_path.c_str(), F_OK) != 0)
        throw CGIException("Invalid CGI file");
    this->createArgs();
    this->runScript();
}

/**
 * @brief Gets the environment variables and checks if all obligatory ones are present
 *
 * @return true if everything is good
 * @return false if an obligatory variable is missing
 */
void CGI::getEnvVars(void) {
    envVars["PATH_INFO"] = getenv("PATH_INFO");
    envVars["QUERY_STRING"] = getenv("QUERY_STRING");
    envVars["CONTENT_LENGTH"] = getenv("CONTENT_LENGTH");
    envVars["CONTENT_TYPE"] = getenv("CONTENT_TYPE");

    if (!envVars["QUERY_STRING"].empty())
        this->parseQueryString();
}

/**
 * @brief checks if all variables are present for the request type
 *
 * @param method either GET, POST or DELETE
 * @return true
 * @return false
 */
void CGI::checkVars(void) {
    if (envVars["PATH_INFO"].empty())
        throw CGIException("PATH_INFO variable missing");

    // if (method == "POST") {
    if (envVars["QUERY_STRING"].empty())
        throw CGIException("QUERY_STRING variable missing");
    if (envVars["CONTENT_LENGTH"].empty())
        throw CGIException("CONTENT_LENGTH variable missing");
    if (envVars["CONTENT_TYPE"].empty())
        throw CGIException("CONTENT_TYPE variable missing");
    //}
}

/**
 * @brief run the CGI and save the output into .cgi_output
 */
void CGI::runScript(void) {
    pid_t pid;

    std::cout << "RUNNER = " << runner << std::endl;
    std::cout << "ARGS = ";
    for (size_t i = 0; args[i]; i += 1)
        std::cout << args[i] << " ";
    std::cout << std::endl;

    int outputFd = open(std::string(".cgi_output").c_str(),
                        O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (outputFd == -1)
        throw CGIException("open() failed");

    pid = fork();
    if (pid == -1)
        throw CGIException("fork() failed");
    // child
    if (pid == 0) {
        dup2(outputFd, STDOUT_FILENO);
        close(outputFd);
        createArgs();
        if (execve(runner.c_str(), args, NULL) == -1)
            throw CGIException("execve() failed");
    } else {
        int status;

        while (waitpid(pid, &status, 0) == -1)
            ;
        close(outputFd);
    }
}

void CGI::createArgs(void) {
    int i = 2;

    if (cgi_ext == ".py") {
        runner = "/usr/bin/python3";
        args = new char*[3 + params.size()];
        args[0] = strdup("python3");
        args[1] = strdup(this->cgi_path.c_str());
        // for (size_t param = 0; param < params.size(); param += 1) {
        //     args[i] = strdup(params[param].c_str());
        //     i += 1;
        // }
        args[i] = NULL;
    }

    else if (cgi_ext == ".php") {
        runner = "/usr/bin/php";
        args = new char*[3 + params.size()];
        args[0] = strdup("php");
        args[1] = strdup(this->cgi_path.c_str());
        // for (size_t param = 0; param < params.size(); param += 1) {
        //     args[i] = strdup(params[param].c_str());
        //     i += 1;
        // }
        args[i] = NULL;
    }
}

/**
 * @brief Parses the query string so that it can be passed as arguments to the script
 */
void CGI::parseQueryString(void) {
    std::string value, param;
    size_t pos, equal;
    std::string qs(envVars["QUERY_STRING"]);

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
}