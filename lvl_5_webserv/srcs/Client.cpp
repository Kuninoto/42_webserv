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

void Client::sendDirectoryListing(std::string path) {
    std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTC-8\nContent-Length: \n\n<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n  <meta charset=\"UTF-8\">\n  <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n  <title>Document</title>\n</head>\n<body>\n";
    DIR *dir;
    struct dirent *ent;
    
    dir = opendir(path.c_str());
    while ((ent = readdir(dir)) != NULL)
    {
        std::string temp(ent->d_name);
        if (temp == "." || temp == "..")
            continue;
        response.append("\t<a href=\"" + (path + ent->d_name) + "\">" + ent->d_name +"</a><br>\n");
    }
    closedir(dir);

    response.insert(71, ft_ntos(response.size() - 78));  
    response.append("</body>\n</html>\r\n\r\n");

    write(this->fd, response.c_str(), response.length());
    request.clear();
}

// get the path to file, this will be useful when join root + index + path
void Client::resolveResponse(std::string& root, std::string& path, size_t safety_cap)
{
    //TODO: better way to do this
    //! error code 508
    if (safety_cap >= 10)
        return;

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
                sendResponse(root + path + "/" + location->second.try_file);
            else if (location->second.auto_index) {
                sendDirectoryListing(root + path.erase(0, 1));
            }
            // else
            //
            //
            // pribnt files
            // error_code forbiden route
            return;

        }
    }

    if (path == "/")
        path += server.getIndex();
    sendResponse(root + path.erase(0, 1));
}

/* void Client::responseFavIcon()
{
    std::ifstream img_file("folhas.png", std::ios::binary | std::ios::in);
    if (!img_file.is_open())
        return;

    std::string response("HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: 85405\r\n\r\n");
    header.append("Content-Type: image/" + getExtension("folhas.png") + "\r\n");

    std::string img_as_binary((std::istreambuf_iterator<char>(img_file)), std::istreambuf_iterator<char>());
    write(this->fd , header.c_str() , header.length());
    write(this->fd , img_as_binary.c_str(), img_as_binary.length());

} */

void Client::sendResponse(std::string path)
{
    std::cout << path << std::endl;
    std::ifstream file(path.c_str(), std::ios::binary | std::ios::in);

    if (!file.is_open())
    {
        write(this->fd, server.getErrorResponse().c_str(), server.getErrorResponse().length());
        request.clear();
        return ;
    }

    std::string response(getHeader(path));

    std::cout << response << std::endl;
    response.append((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());	

    write(this->fd, response.c_str(), response.length());
    request.clear();
}

void Client::response()
{
    // if already sent or request is not complete return
    if (request_sent || request.find(REQUEST_DELIMITER) == std::string::npos)
        return;
    request_sent = true;

    parseRequest();
    
    std::string root = server.getRoot();
    std::string path = this->page;
    resolveResponse(root, path, 0);
    /* if (type == FAVICON)
    {
        responseFavIcon();
        return;
    }
    */
}
