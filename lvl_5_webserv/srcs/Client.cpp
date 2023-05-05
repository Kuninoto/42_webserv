#include "Client.hpp"
#include "CGI.hpp"

#include <dirent.h>

#include <algorithm>
#include <sstream>

Client::Client(Server server, int fd) : server(server), fd(fd), request_sent(false){};

void Client::setRequest(std::string request) {
	this->request_sent = false;
	this->request.append(request);
}

void Client::parseRequest(void) {
	this->request_content.clear();
	std::string line;
	std::stringstream ss(this->request);

	std::getline(ss, line);
	components = splitStr(line, ' ');
	if (components.size() != 3)
		throw ClientException(RS400);

	std::map<std::string, std::string>::iterator it;

	std::cout << "DEBUG: " << this->request << std::endl;

	// Set the method of the request if it's a valid one (GET, POST, or DELETE)
	if (components.at(0) == "GET" || components.at(0) == "POST" || components.at(0) == "DELETE")
		this->method = components.at(0);
	else
		throw ClientException(RS501);

	// Set the target URI of the request
	this->uri_target = components.at(1);
	// If the length of the URI is greater than 1024 characters, throw a client exception with the response code RS414
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
			// If the line contains a colon character (indicating a header), split it into name and content
			std::string name(line.substr(0, line.find(':')));
			std::string content(line.substr(line.find(':') + 2, line.find('\n')));
			// If the content of the header is not empty, add it to the headers map
			if (content.length() != 0)
				this->headers[name] = content;
			else
				// If the content of the header is empty, throw a client exception with the response code RS400
				throw ClientException(RS400);
		}
	}

	// Read the rest of the request content
	//* GET requests can also have query strings
	while (std::getline(ss, line))
		this->request_content += line;

	// printMap(headers);
	createEnvVars();

	// If the method of the request is POST, parse the request content (if any)
	if (this->method == "POST")
		handlePostRequest();
}

void Client::handlePostRequest() {
	// Get the content type from the headers map
	std::string contentType = this->headers["Content-Type"];

	std::cout << "DEBUG: " << contentType << std::endl;
	
	// Check if the content type is JSON
	if (contentType == "application/x-www-form-urlencoded") {
	
			std::cout << "Running CGI..." << std::endl;
			CGI cgi;
			std::cout << "CGI finished running" << std::endl;
			// Send a response back to the client indicating success
			std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
			this->sendResponse(response);
	}
	else {
		std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\n";
		this->sendResponse(response);
	}
}


void Client::createEnvVars() {
	setenv("REQUEST_METHOD", method.c_str(), 1);
	setenv("SCRIPT_NAME", components.at(1).c_str(), 1);
	setenv("PATH_INFO", server.getRoot().c_str(), 1);
	setenv("QUERY_STRING", request_content.c_str(), 1);
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

	const std::string& response = getResponseBoilerPlate(RS200, this->uri_target.erase(0, 1), body);

	write(this->fd, response.c_str(), response.length());
	request.clear();
}

void Client::sendResponse(std::string uri) {
	messageLog(this->method + " " + uri, RESET, false);
	std::ifstream file(uri.c_str(), std::ios::binary | std::ios::in);

	if (!file.is_open()) {
		write(this->fd, server.getErrorResponse().c_str(), server.getErrorResponse().length());
		request.clear();
		return;
	}

	std::string response(getOkHeader(uri));
	response.append((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	write(this->fd, response.c_str(), response.length());
	request.clear();
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

	const std::string& response = getResponseBoilerPlate(code, code, body);

	write(this->fd, response.c_str(), response.length());
	this->request.clear();
}

void Client::resolveResponse(std::string& root, std::string& uri, size_t safety_cap) {
	size_t locate;

	if (safety_cap >= 20)
		throw ClientException(RS508);

	if (uri == "/favicon.ico") {
		sendResponse("pages/favicon.png");
		return;
	}

	locationMap::const_iterator location;

	for (location = server.getLocations().begin(); location != server.getLocations().end(); location++) {
		if (location->first == "/" && uri != "/") continue;

		if ((locate = uri.find(location->first)) == std::string::npos) continue;

		if (location->second.allowed_methods.size() != 0 && std::find(location->second.allowed_methods.begin(),
																	  location->second.allowed_methods.end(),
																	  this->method) == location->second.allowed_methods.end())
			throw ClientException(RS405);

		if (location->second.redirect.size()) {
			uri.erase(locate, location->first.size())
				.insert(locate, location->second.redirect);
			resolveResponse(root, uri, safety_cap + 1);
			return;
		}

		if (location->second.root.size()) {
			uri.erase(locate, location->first.size());
			root = location->second.root;
		}

		if (isDirectory((root + uri).c_str())) {
			if (location->second.try_file.size())
				sendResponse(root + uri + "/" + location->second.try_file);
			else if (location->second.auto_index)
				sendDirectoryListing(root + uri.erase(0, 1));
			else if (uri == "/")
				sendResponse(root + server.getIndex());
			else
				throw ClientException(RS403);
			return;
		}
	}

	if (uri == "/")
		uri += server.getIndex();
	if (isDirectory((root + uri).c_str()))
		throw ClientException(RS403);
	sendResponse(root + uri.erase(0, 1));
}

void Client::response(void) {
	// if already sent or request is not complete return
	this->request_sent = true;

	try {
		this->parseRequest();
		std::string root = this->server.getRoot();
		std::string uri = this->uri_target;
		this->resolveResponse(root, uri, 0);
	} catch (const std::exception& e) {
		this->sendErrorCode(e.what());
	}
}
