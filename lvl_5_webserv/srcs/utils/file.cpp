#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "utils.hpp"

bool isRegularFile(const char* path) {
    struct stat st;

    if (stat(path, &st) == 0 && S_ISREG(st.st_mode)) {
        return true;
    }
    return false;
}

bool isDirectory(const char* path) {
    struct stat st;

    if (stat(path, &st) != 0) {
        return false;
    }
    return st.st_mode & S_IFDIR;
}

std::string getFileType(const std::string& file) {
    static std::map<std::string, std::string> types;

    types["txt"] = "text/plain";
    types["html"] = "text/html";
    types["css"] = "text/css";

    types["js"] = "application/javascript";
    types["py"] = "application/python";

    types["jpg"] = "image/jpg";
    types["jpeg"] = "image/jpeg";
    types["png"] = "image/png";
    types["gif"] = "image/gif";

    if (types.count(file.substr(file.find_last_of(".") + 1)) > 0)
        return types[file.substr(file.find_last_of(".") + 1)];
    return "text/plain";
}

std::string getHTMLBoilerPlate(const std::string& code, const std::string& title, const std::string& body) {
    std::string html =
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <title>" +
        title +
        "</title>\n"
        "</head>\n"
        "<body>\n    " +
        body +
        "\n</body>\n"
        "</html>\n";

    std::string response =
        "HTTP/1.1 " + code +
        "\n"
        "Date: " +
        getTimeStamp() +
        "\n"
        "Server: 42_Webserv/1.0 (Linux)\n" +
        "Content-Type: text/html; charset=UTF-8\n" +
        "Content-Length: " + ft_ntos(html.size()) + "\n\n" + html;
    return response;
}

std::string getFileSize(const std::string& file) {
    std::streampos begin, end;
    std::stringstream stream;
    std::string size;

    std::ifstream open_file(file.c_str(), std::ios::binary | std::ios::in);

    begin = open_file.tellg();
    open_file.seekg(0, std::ios::end);
    end = open_file.tellg();
    open_file.close();

    stream << end - begin;
    stream >> size;

    return size;
}

std::string getOkHeader(const std::string& file) {
    std::string header =
        "HTTP/1.1 200 OK\n"
        "Date: " +
        getTimeStamp() +
        "\n"
        "Server: 42_Webserv/1.0 (Linux)\n" +
        "Connection: keep-alive\n" +
        "Content-Type: " +
        getFileType(file) + "; charset=UTF-8\n" +
        "Content-Length: " + getFileSize(file) + "\n\n";

    return header;
}

std::string getFileContent(const std::string& file) {
    std::ifstream fileStream(file.c_str(), std::ios::binary | std::ios::in);
    std::string fileContent;

    fileContent.append((std::istreambuf_iterator<char>(fileStream)),
                       std::istreambuf_iterator<char>());
    return fileContent;
}
