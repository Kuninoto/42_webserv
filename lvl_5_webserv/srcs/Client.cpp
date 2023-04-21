# include "Client.hpp"
# include <sstream>

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

    if (components.at(1).find("html") != std::string::npos)
        type = HTML;
    else if (components.at(1).find("png") != std::string::npos)
        type = IMAGE;
    else if (components.at(1).find("/favicon.ico") != std::string::npos)
        type = FAVICON;
    else
        type = TEXT;

}

// get the path to file, this will be usefull when join root + index + path
std::string Client::getPathToPage()
{
    std::map<std::string, location_t>::const_iterator location;
    std::string path = server.getRoot();

/*     for (location = server.getLocations().begin(); location != server.getLocations().end(); location++)
    {
        if (page.find(location->first) != std::string::npos)
        {
            
        }

        std::cout << location->first << std::endl;
    } */
/*     std::cout << "--" << std::endl; */

    if (page == "/")
        return path + server.getIndex();
    return path + page.erase(0, 1);
}

/* void Client::responseFavIcon()
{
    std::ifstream img_file("folhas.png", std::ios::binary | std::ios::in);
    if (!img_file.is_open())
        return;

    std::string header("HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: 85405\r\n\r\n");
    header.append("Content-Type: image/" + getExtension("folhas.png") + "\r\n");
2
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
    /* if (type == FAVICON)
    {
        responseFavIcon();
        return;
    }
    */
    std::string path_to_file = getPathToPage();
    std::ifstream file(path_to_file.c_str(), std::ios::binary | std::ios::in);

    if (!file.is_open())
    {
        write(this->fd , server.getErrorResponse().c_str() , server.getErrorResponse().length());
        request.clear();
        return ;
    }

    std::string header(getHeader(path_to_file));

    header.append((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());	

    write(this->fd , header.c_str() , header.length());
    request.clear();
}
