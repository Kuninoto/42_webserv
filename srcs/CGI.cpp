#include "CGI.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

#include "utils.hpp"

using std::cout;
using std::endl;

#define WRITE_END 1
#define READ_END 0

static std::string getScriptFileName(const std::vector<std::string> envVars) {
    std::vector<std::string>::const_iterator v_it;

    for (v_it = envVars.begin(); v_it != envVars.end(); v_it++) {
        size_t pos = v_it->find("SCRIPT_FILENAME=");
        if (pos != std::string::npos) {
            return v_it->substr(pos + 16);
        }
    }
    return "";
};

CGI::CGI(const std::string& cgi_ext, const std::string& request,
         const std::vector<std::string>& envVars, size_t bodyLength,
         const std::string& uploadTo)
    : request(request), bodyLength(bodyLength), uploadTo(uploadTo) {
    this->cgi_path = getScriptFileName(envVars);
    if (cgi_path.empty())
        throw CGIException("No SCRIPT_FILENAME provided");
    this->cgi_ext = cgi_ext;

    size_t dotPos = cgi_path.find_last_of('.');
    if (dotPos == std::string::npos)
        throw CGIException("No extension. Expected \"" + cgi_ext + "\"");

    std::string ext = cgi_path.substr(dotPos);
    if (ext != cgi_ext)
        throw CGIException("Invalid extension \"" + ext + "\" expected \"" + cgi_ext + "\"");

    if (access(this->cgi_path.c_str(), F_OK) != 0)
        throw CGIException("Invalid CGI file");
    this->createArgvAndEnvp(envVars);
    this->runScript();
}

CGI::~CGI(void) {
    remove(".cgi_output");
    for (size_t i = 0; this->argv[i]; i += 1)
        free(this->argv[i]);
    delete[] this->argv;

    for (size_t i = 0; this->envp[i]; i += 1)
        free(this->envp[i]);
    delete[] this->envp;
}

/**
 * @brief run the CGI and save the output into .cgi_output
 */
void CGI::runScript(void) {
    pid_t pid;

    // !TODO
    // perhaps change to tmpfile()
    int outputFd = open(".cgi_output", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (outputFd == -1)
        throw CGIException("open() failed");

    int pipedes[2];
    if (pipe(pipedes) == -1)
        throw CGIException("pipe() failed");

    if (this->bodyLength > 0)
        fcntl(pipedes[WRITE_END], F_SETPIPE_SZ, this->bodyLength);

    write(pipedes[WRITE_END], this->request.data(), this->bodyLength);
    close(pipedes[WRITE_END]);

    pid = fork();
    if (pid == -1)
        throw CGIException("fork() failed");

    // child
    if (pid == 0) {
        dup2(pipedes[READ_END], STDIN_FILENO);
        close(pipedes[READ_END]);

        dup2(outputFd, STDOUT_FILENO);
        close(outputFd);

        if (execve(runner.c_str(), this->argv, this->envp) == -1)
            throw CGIException("execve() failed");
    } else {
        int status;
        close(pipedes[READ_END]);
        close(outputFd);
        waitpid(pid, &status, 0);
    }
}

void CGI::createArgvAndEnvp(const std::vector<std::string>& envVars) {
    if (cgi_ext == ".py") {
        runner = "/usr/bin/python3";

        argv = new char*[4];
        argv[0] = strdup("python3");
        argv[1] = strdup(this->cgi_path.c_str());
        argv[2] = strdup(this->uploadTo.c_str());
        argv[3] = NULL;
    }

    this->envp = new char*[envVars.size() + 1];

    size_t i = 0;
    std::vector<std::string>::const_iterator v_it;
    for (v_it = envVars.begin(); v_it != envVars.end(); v_it++, i++) {
        this->envp[i] = strdup(v_it->c_str());
    }
    this->envp[i] = NULL;
}