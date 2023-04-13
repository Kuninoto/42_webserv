#include "Client.hpp"

Client::Client(int fd): fd(fd), request_sent(false){}

void Client::setRequest(std::string request)
{
    this->request_sent = false;
    this->request.append(request); 
}

void Client::response()
{
    // if already sent or request is not complete return
    if (request_sent || request.find("\r\n\r\n") == std::string::npos)
        return;
    request_sent = true;

    std::string line;
    std::ifstream request_steam(request.c_str());

    getline(request_steam, line);

    std::cout << request << std::endl;


    std::string coisa;
    //? GET [name]
    //? [name] is the file to open
    if (request.find("folhas") != std::string::npos)
        coisa = "pages/index.html";
    else
        coisa = "asdiskgioearg";

    std::ifstream img_file(coisa.c_str(), std::ios::binary | std::ios::in);

    if (!img_file.is_open())
    {
        std::ifstream file("pages/error/404.html", std::ios::binary | std::ios::in);
        std::string hello((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());	

        write(this->fd , hello.c_str() , hello.length());
        return ;
    }

    //std::string header("HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: 85405\r\n\r\n");
    //std::string img((std::istreambuf_iterator<char>(img_file)), std::istreambuf_iterator<char>());

    std::string hello((std::istreambuf_iterator<char>(img_file)), std::istreambuf_iterator<char>());	
    write(fd , hello.c_str() , hello.length());
    //write(fd , img.c_str(), img.length());
    request.clear();
}
