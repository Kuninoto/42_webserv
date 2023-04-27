# include "Client.hpp"
# include <sstream>
# include <algorithm>
# include <dirent.h>

Client::Client(Server server, int fd): server(server), fd(fd), request_sent(false){}

void Client::setRequest(std::string request)
{
    this->request_sent = false;
    this->request.append(request); 
}

void Client::parseRequest()
{
    std::string line;
    std::stringstream request_stream(request);

    std::getline(request_stream, line);
    std::vector<std::string> components = splitStr(line, ' ');

    if (components.at(0) == "GET")
        method = GET;
    else if (components.at(0) == "POST")
        method = POST;
    else if (components.at(0) == "DELETE")
        method = DELETE;
    else
        std::cout << "error" << std::endl;

    page = components.at(1);
}

void Client::sendDirectoryListing(std::string path)
{
    std::string body;
    DIR *dir;
    struct dirent *ent;
    
    dir = opendir(path.c_str());
    while ((ent = readdir(dir)) != NULL)
    {
        std::string temp(ent->d_name);
        if (temp == "." || temp == "..")
            continue;
        body.append("\t<a href=\"" + this->page + "/" + ent->d_name + "\">" + ent->d_name +"</a><br>\n");
    }
    closedir(dir);

    const std::string& response = getResponseBoilerPlate("200 OK", path, body);

    std::cout << "RESPONSE: " << response << std::endl;
    write(this->fd, response.c_str(), response.length());
    request.clear();
}

void Client::sendResponse(std::string path, std::string code)
{
    std::cout << path << std::endl;
    std::ifstream file(path.c_str(), std::ios::binary | std::ios::in);

    if (!file.is_open())
    {
        write(this->fd, server.getErrorResponse().c_str(), server.getErrorResponse().length());
        request.clear();
        return;
    }

    std::string response(getHeader(path, code));

    std::cout << response << std::endl;
    response.append((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());	

    write(this->fd, response.c_str(), response.length());
    request.clear();
}

void Client::sendErrorCode(std::string code)
{
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
    request.clear();

}

// get the path to file, this will be useful when join root + index + path
void Client::resolveResponse(std::string& root, std::string& path, size_t safety_cap)
{
    if (safety_cap >= 20)
        throw ClientException("508 Loop Detected");

    locationMap::const_iterator location;

    for (location = server.getLocations().begin(); location != server.getLocations().end(); location++)
    {
        if (location->first == "/") continue;

        std::string::size_type locate = path.find(location->first);
        if (locate == std::string::npos) continue;

        if (location->second.redirect.size())
        {
            path.erase(locate, location->first.size())
                .insert(locate, location->second.redirect);
            resolveResponse(root, path, safety_cap + 1);
            return ;
        }

        if (location->second.root.size())
        {
            path.erase(locate, location->first.size());
            root = location->second.root;
        }

        if (isDirectory((root + path).c_str()))
        {
            if (location->second.try_file.size())
            {
                sendResponse(root + path + "/" + location->second.try_file, "200");
                return;
            }
            else if (location->second.auto_index)
                sendDirectoryListing(root + path.erase(0, 1));
            else
                throw ClientException("403 Forbidden"); 
        }
    }

    if (path == "/")
        path += server.getIndex();
    sendResponse(root + path.erase(0, 1), "200");
}

/* void Client::responseFavIcon()
{
    std::ifstream img_file("folhas.png", std::ios::binary | std::ios::in);
    if (!img_file.is_open())
        return;

    std::string response("HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: 85405\r\n\r\n"); header.append("Content-Type: image/" + getExtension("folhas.png") + "\r\n");

    std::string img_as_binary((std::istreambuf_iterator<char>(img_file)), std::istreambuf_iterator<char>());
    write(this->fd , header.c_str() , header.length());
    write(this->fd , img_as_binary.c_str(), img_as_binary.length());

} */

void Client::response()
{
    // if already sent or request is not complete return
    if (request_sent || request.find(REQUEST_DELIMITER) == std::string::npos)
        return;
    request_sent = true;

    parseRequest();

    try
    {
        std::string root = server.getRoot();
        std::string path = this->page;
        resolveResponse(root, path, 0);
    }
    catch(const std::exception& e)
    {
        sendErrorCode(e.what());
    }
    

    /* if (type == FAVICON)
    {
        responseFavIcon();
        return;
    }
    */
}
