#include "Client.hpp"

#include <dirent.h>

#include <algorithm>
#include <sstream>

#include "CGI.hpp"

Client::Client(Server server, int fd) : server(server), fd(fd), request_sent(false){};

void Client::setRequest(std::string request) {
    this->request_sent = false;
    this->request.append(request);
}

void Client::parseRequest(void) {
    this->request_content.clear();
    std::string line;
    std::stringstream ss(this->request);
    std::vector<std::string> components;

    std::getline(ss, line);
    components = splitStr(line, ' ');
    if (components.size() != 3)
        throw ClientException(RS400);

    // Set the method of the request if it's a valid one (GET, POST, or DELETE)
    if (components.at(0) == "GET" || components.at(0) == "POST" || components.at(0) == "DELETE")
        this->method = components.at(0);
    else
        throw ClientException(RS501);

    // Set the target URI of the request
    this->uri_target = components.at(1);
    // if URI Too Large
    if (this->uri_target.length() > 1024)
        throw ClientException(RS414);

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
            if (content.length() != 0)
                this->headers[name] = content;
            else
                // If the content of the header is empty
                // send Bad Request
                throw ClientException(RS400);
        }
    }

    //checkHeaders()

    // Read the rest of the request content
    // GET requests can also have query strings
    while (std::getline(ss, line))
        this->request_content += line;
}

void Client::handleGetRequest(std::string& root, std::string& uri) {
    if (uri == "/favicon.ico") {
        sendResponse("pages/favicon.png");
        return;
    }

    if (uri == "/")
        uri += server.getIndex();
    if (isDirectory((root + uri).c_str()))
        throw ClientException(RS403);
    sendResponse(root + uri.erase(0, 1));
}

void Client::handlePostRequest(std::string& root, std::string& uri, const location_t& targetLocation) {
    std::string response;

    try {
        //! TODO
        // 400 (?)
        // if the target location doesn't have a CGI configured
        if (!targetLocation.hasCGI)
            throw ClientException(RS400);
        createEnvVars(uri);
        CGI cgi(root + targetLocation.cgi_path
                     + uri.erase(0, 1)
                          .substr(uri.find_last_of("/")), targetLocation.cgi_ext);

        response = getHTMLBoilerPlate(RS200, "OK", getFileContent(".cgi_output"));
    
        write(this->fd, response.c_str(), response.length());
        logMessage(this->method + " " + uri + GREEN + " -> 200 OK");
    } catch (const std::exception& e) {
        response = server.getErrorResponse();
        write(this->fd, response.c_str(), response.length());
        logMessage(this->method + " " + uri + RED + " -> 404 Not Found");
        std::cerr << "CGI Error: " << e.what() << '\n';
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

void Client::createEnvVars(std::string uri) {
    //! TODO
    // Review/Research this info
    // PATH_INFO saves the rest of the url past the CGI
    // example: cgi-bin/process_form.py/yada
    // PATH_INFO = yada
    setenv("PATH_INFO", uri.erase(0, 1).substr(uri.find_first_of("/")).c_str(), 1);
    std::cout << "PATH_INFO = " << getenv("PATH_INFO") << std::endl;

    setenv("QUERY_STRING", request_content.c_str(), 1);
    std::cout << "QUERY_STRING = " << getenv("QUERY_STRING") << std::endl;

    setenv("CONTENT_LENGTH", headers["Content-Length"].c_str(), 1);
    setenv("CONTENT_TYPE", headers["Content-Type"].c_str(), 1);
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
        body.append("\t<a href=\"" + this->uri_target + "/" + ent->d_name + "\">" + ent->d_name + "</a><br>\n");
    }
    closedir(dir);

    const std::string& response = getHTMLBoilerPlate(RS200, this->uri_target.erase(0, 1), body);

    write(this->fd, response.c_str(), response.length());
    logMessage(this->method + " " + uri + GREEN + " -> 200 OK");
    request.clear();
}

void Client::sendResponse(std::string uri) {
    std::ifstream file(uri.c_str(), std::ios::binary | std::ios::in);

    if (!file.is_open()) {
        request.clear();
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
    body += "\t<div style='margin: 1rem 0;'>Random ass picture:</div>\n";
    body += "\t<img src='https://picsum.photos/200' alt='random ass'>\n";
    body += "</div>\n";
    body += "<style>\n";
    body += "\tbody {\n";
    body += "\t\tdisplay: flex;\n";
    body += "\t\twidth: 100vw;\n";
    body += "\t\theight: 100vh;\n";
    body += "\t}\n</style>\n";

    const std::string& response = getHTMLBoilerPlate(code, code, body);
    write(this->fd, response.c_str(), response.length());

    this->request.clear();
}

void Client::resolveLocation(std::string& root, std::string& uri, size_t safety_cap) {
    if (safety_cap >= 20)
        throw ClientException(RS508);

    size_t locate;
    locationMap::const_iterator tempLocation;
    locationMap::const_iterator targetLocation;
    for (tempLocation = server.getLocations().begin(); tempLocation != server.getLocations().end(); tempLocation++) {
        targetLocation = tempLocation;
        if (tempLocation->first == "/" && uri != "/") continue;

        if (uri.find(tempLocation->first + '/') == std::string::npos
        &&  !endsWith(uri, tempLocation->first))
            continue;
        locate = uri.find(tempLocation->first);

        // if no allow_methods are set or method is forbidden
        if (tempLocation->second.allowed_methods.size() != 0
        && std::find(tempLocation->second.allowed_methods.begin(),
                     tempLocation->second.allowed_methods.end(),
                     this->method) == tempLocation->second.allowed_methods.end())
            throw ClientException(RS405);

        if (tempLocation->second.redirect.size()) {
            uri.erase(locate, tempLocation->first.size())
               .insert(locate, tempLocation->second.redirect);
            //! TODO
            // this->resolveResponse()?
            this->resolveLocation(root, uri, safety_cap);
            return;
        }

        if (tempLocation->second.root.size()) {
            uri.erase(locate, tempLocation->first.size());
            root = tempLocation->second.root;
        }

        if (isDirectory((root + uri).c_str())) {
            if (tempLocation->second.try_file.size())
                this->sendResponse(root + uri + "/" + tempLocation->second.try_file);
            else if (tempLocation->second.auto_index)
                this->sendDirectoryListing(root + uri.erase(0, 1));
            else if (uri == "/")
                this->sendResponse(root + server.getIndex());
            else
                throw ClientException(RS403);
            return;
        }
    }
    this->resolveResponse(root, uri, targetLocation->second);
}

void Client::resolveResponse(std::string& root, std::string& uri, const location_t& targetLocation) {

    if (this->method == "GET") {
        handleGetRequest(root, uri);
    } else if (this->method == "POST") {
        handlePostRequest(root, uri, targetLocation);
    } else if (this->method == "DELETE") {
        handleDeleteRequest(root, uri);
    }
    request.clear();
}

void Client::response(void) {
    if (!this->preparedToSend())
        return;
    this->request_sent = true;

    try {
        this->parseRequest();

        std::string root = this->server.getRoot();
        std::string uri = this->uri_target;

        this->resolveLocation(root, uri, 0);
    } catch (const std::exception& e) {
        this->sendErrorCode(e.what());
        std::cout << "-> " << e.what() << std::endl;
    }
}
