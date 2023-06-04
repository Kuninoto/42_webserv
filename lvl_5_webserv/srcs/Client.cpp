#include "Client.hpp"

#include <dirent.h>

#include <algorithm>
#include <sstream>

#include "CGI.hpp"
#include "utils.hpp"

Client::Client(Server server, int fd) : server(server), fd(fd), request_sent(false){};

void Client::setRequest(const char* chunk, size_t bufferLength) {
    this->last_request = std::time(NULL);
    this->request_sent = false;
    this->request.append(chunk, bufferLength);
}

void Client::parseRequest(void) {
    this->requestBody.clear();
    this->headers.clear();
    std::string line;
    std::stringstream ss(this->request);
    std::vector<std::string> components;

    std::getline(ss, line);
    components = splitStr(line, ' ');
    if (components.size() != 3)
        throw ClientException(RS400);

    // Set the method of the request and throw 501 if it's not an implemented one (GET, POST, or DELETE)
    this->method = components.at(0);
    if (!(components.at(0) == "GET" || components.at(0) == "POST" || components.at(0) == "DELETE"))
        throw ClientException(RS501);

    // Set the target URI of the request
    this->uri_target = components.at(1);
    // if URI Too Large
    if (this->uri_target.length() > 1024)
        throw ClientException(RS414);

    // if URI contains ../ or is literally ..
    if (this->uri_target.find("../") != std::string::npos || this->uri_target == "..")
        throw ClientException(RS400);

    // Check if the HTTP version is valid (must be HTTP/1.1)
    if (components.at(2) == "HTTP/1.0\r")
        throw ClientException(RS505);
    if (components.at(2) != "HTTP/1.1\r")
        throw ClientException(RS400);

    // Parse the headers of the request
    while (std::getline(ss, line) && line != "\r") {
        if (line.find(':') != std::string::npos) {
            // If the line contains a colon character (indicating a header),
            // split it into name and content
            std::string name(line.substr(0, line.find(':')));
            std::string content(line.substr(line.find(':') + 2, line.find('\n')));

            // If the content of the header is not empty, add it to the headers map
            if (content.length() != 0) {
                trimStr(content, "\r\n");
                this->headers[name] = content;
            } else
                // If the content of the header is empty
                // send Bad Request
                throw ClientException(RS400);
        }
    }

    // GET and DELETE don't have body
    // so parsing ends here
    if (method == "GET" || method == "DELETE")
        return;

    this->bodyLength = ft_stoul(headers["Content-Length"]);
    if (this->bodyLength > this->server.getMaxBodySize())
        throw ClientException(RS413);

    // Get ss read position
    std::streampos tempPos = ss.tellg();

    // open a new ss with the same content as ss but with binary mode
    std::stringstream binary_ss(ss.str(), std::stringstream::in | std::stringstream::binary);

    // move binary_ss read position to tempPos (ss read position)
    binary_ss.seekg(tempPos);

    // resize requestBody to bodyLength so that
    // we can read directly to the its storage
    // bodyLength characters
    this->requestBody.resize(this->bodyLength);

    binary_ss.read(&requestBody[0], this->bodyLength);
}

void Client::handleGetRequest(std::string& root, std::string& uri, const location_t& targetLocation) {
    if (*(uri.end() - 1) == '?') {
        uri.erase(uri.end() - 1);
        std::string response;
        try {
            CGI cgi(".py", requestBody,
                    createEnvVars(targetLocation.root, uri, targetLocation),
                    0, "");
            response = getHTMLBoilerPlate(RS200, "OK", getFileContent(".cgi_output"));
            write(this->fd, response.c_str(), response.length());
            logMessage(this->method + " " + uri + GREEN + " -> 200 OK");
        } catch (const std::exception& e) {
            response = getHTMLBoilerPlate(RS500, "Internal Server Error", "<h1>500 Internal Server Error</h1>");
            write(this->fd, response.c_str(), response.length());
            logMessage(this->method + " " + uri + RED + " -> 500 Internal Server Error");
            std::cerr << e.what() << '\n';
        }
    }

    if (uri == "/")
        uri += server.getIndex();
    if (isDirectory((root + uri).c_str())) {
        throw ClientException(RS403);
    }
    sendGetResponse(root + uri.erase(0, 1));
}

void Client::handlePostRequest(std::string& root, std::string& uri, const location_t& targetLocation) {
    std::string response;

    if (targetLocation.uploadTo.empty() && !targetLocation.hasCGI) {
        throw ClientException(RS405);
    }

    // !TODO
    // review which root is needed
    (void)root;

    try {
        CGI cgi(".py", requestBody,
                createEnvVars(targetLocation.root, uri, targetLocation),
                this->bodyLength, targetLocation.root + targetLocation.uploadTo);
        response = getHTMLBoilerPlate(RS200, "OK", getFileContent(".cgi_output"));
        write(this->fd, response.c_str(), response.length());
        logMessage(this->method + " " + uri + GREEN + " -> 200 OK");
    } catch (const std::exception& e) {
        response = getHTMLBoilerPlate(RS500, "Internal Server Error", "500 Internal Server Error");
        write(this->fd, response.c_str(), response.length());
        logMessage(this->method + " " + uri + RED + " -> 500 Internal Server Error");
        std::cerr << e.what() << '\n';
    }
}

void Client::handleDeleteRequest(std::string& root, std::string& uri) {
    std::string filename = root + uri;
    std::string response;

    if (remove(filename.c_str()) == 0) {
        response = getHTMLBoilerPlate(RS200, "OK", "<h1>File deleted.</h1>\n");
        write(this->fd, response.c_str(), response.length());
        logMessage(this->method + " " + uri + GREEN + " -> 200 OK");
    } else {
        response = server.getErrorResponse();
        write(this->fd, response.c_str(), response.length());
        logMessage(this->method + " " + uri + RED + " -> 404 Not Found");
    }
}

std::vector<std::string> Client::createEnvVars(const std::string& serverRoot, std::string uri, const location_t& targetLocation) {
    std::vector<std::string> envVars;

    if (uri.at(0) == '/')
        uri.erase(0, 1);

    envVars.push_back("SCRIPT_FILENAME=" + (serverRoot + targetLocation.cgi_path + uri));

    if (headers.count("Content-Length") > 0) {
        envVars.push_back("CONTENT_LENGTH=" + headers["Content-Length"]);
    }

    envVars.push_back("CONTENT_TYPE=" + headers["Content-Type"]);
    envVars.push_back("GATEWAY_INTERFACE=CGI/1.1");
    envVars.push_back("REQUEST_METHOD=" + this->method);
    envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
    envVars.push_back("SERVER_SOFTWARE=42_Webserv/1.0");

    return envVars;
}

void Client::sendDirectoryListing(std::string uri) {
    std::string body;
    DIR* dir;
    struct dirent* ent;

    dir = opendir(uri.c_str());
    while ((ent = readdir(dir)) != NULL) {
        std::string temp(ent->d_name);
        if (temp == "." || temp == "..")
            continue;
        body.append("\t<a href=\"" + this->uri_target + "/" + ent->d_name + "\">" + ent->d_name + "</a>\n<br>\n");
    }
    closedir(dir);

    const std::string& response = getHTMLBoilerPlate(RS200, this->uri_target.erase(0, 1), body);

    write(this->fd, response.c_str(), response.length());
    logMessage(this->method + " " + uri + GREEN + " -> 200 OK");
    request.clear();
}

void Client::sendGetResponse(std::string uri) {
    std::ifstream file(uri.c_str(), std::ios::binary | std::ios::in);

    if (!file.is_open()) {
        write(this->fd, server.getErrorResponse().c_str(), server.getErrorResponse().length());
        logMessage(this->method + " " + uri + RED + " -> 404 Not Found");
        return;
    }

    std::string response(getOkHeader(uri));
    response.append((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    write(this->fd, response.c_str(), response.length());
    logMessage(this->method + " " + uri + GREEN + " -> 200 OK");
}

void Client::sendErrorCode(std::string code) {
    std::string body;

    body += "<div style='margin: auto; text-align: center;'>\n";
    body += "\t<div style='font-size: 72px'>" + code + "</div>\n";
    body += "\t<img src='https://picsum.photos/200' alt='beautiful picture'>\n";
    body += "</div>\n";
    body += "<style>\n";
    body += "\tbody {\n";
    body += "\t\tdisplay: flex;\n";
    body += "\t\twidth: 100vw;\n";
    body += "\t\theight: 100vh;\n";
    body += "\t}\n</style>\n";

    const std::string& response = getHTMLBoilerPlate(code, code, body);
    write(this->fd, response.c_str(), response.length());
}

void Client::resolveLocation(std::string& root, std::string& uri, size_t safety_cap) {
    if (safety_cap >= 10)
        throw ClientException(RS508);

    size_t locate;
    locationMap::const_iterator location;
    for (location = server.getLocations().begin(); location != server.getLocations().end(); location++) {
        if (location->first == "/" && uri != "/") continue;

        if (uri.find(location->first + '/') == std::string::npos && !endsWith(uri, location->first))
            continue;
        locate = uri.find(location->first);

        // if no allow_methods are set or method is forbidden
        if (location->second.allowed_methods.size() != 0 && std::find(location->second.allowed_methods.begin(),
                                                                      location->second.allowed_methods.end(),
                                                                      this->method) == location->second.allowed_methods.end())
            throw ClientException(RS405);

        if (location->second.redirect.size()) {
            uri.erase(locate, location->first.size())
                .insert(locate, location->second.redirect);
            this->resolveLocation(root, uri, safety_cap + 1);
            return;
        }

        if (location->second.root.size()) {
            uri.erase(locate, location->first.size());
            root = location->second.root;
        }

        if (isDirectory((root + uri).c_str())) {
            if (location->second.try_file.size())
                this->sendGetResponse(root + uri + "/" + location->second.try_file);
            else if (location->second.auto_index) {
                this->sendDirectoryListing(root + uri.erase(0, 1));
            } else if (uri == "/")
                this->sendGetResponse(root + server.getIndex());
            else {
                throw ClientException(RS403);
            }
            return;
        }
        break;
    }
    if (location == server.getLocations().end() && (this->method == "POST" || this->method == "DELETE")) {
        throw ClientException(RS403);
    }

    // Requests to CGI
    if ((this->method == "POST") || (this->method == "GET" && *(uri.end() - 1) == '?')) {
        size_t locate = uri.find(location->first);
        if (locate != std::string::npos)
            uri.erase(locate, location->first.size());
    }
    this->resolveResponse(root, uri, location->second);
}

void Client::resolveResponse(std::string& root, std::string& uri, const location_t& targetLocation) {
    if (this->method == "GET") {
        handleGetRequest(root, uri, targetLocation);
    } else if (this->method == "POST") {
        handlePostRequest(root, uri, targetLocation);
    } else if (this->method == "DELETE") {
        handleDeleteRequest(root, uri);
    }
}

void Client::response(void) {
    if (!this->preparedToSend())
        return;
    this->request_sent = true;
    this->last_request = std::time(NULL);

    try {
        this->parseRequest();
        std::string root = this->server.getRoot();
        std::string uri = this->uri_target;

        this->resolveLocation(root, uri, 0);

    } catch (const std::exception& e) {
        this->sendErrorCode(e.what());
        logMessage(this->method + " " + this->uri_target + RED + " -> " + e.what());
    }
    request.clear();
    requestBody.clear();
}
