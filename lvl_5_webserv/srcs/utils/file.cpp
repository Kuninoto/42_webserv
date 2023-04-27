# include <string>
# include <map>
# include <fstream>
# include <iostream>
# include <sstream>
# include <dirent.h>
# include <sys/stat.h>
# include <unistd.h>
# include "utils.hpp"

bool isRegularFile(const char *path)
{
	struct stat st;
	if (stat(path, &st) == 0
    && S_ISREG(st.st_mode)) {
		return true;
	}
	return false;
}


bool isDirectory(const char *path)
{
	struct stat st;
    
    if (stat(path,&st) != 0) {
        return false;
    }

	return st.st_mode & S_IFDIR;
}

std::string getFileType(std::string file)
{
    static std::map<std::string, std::string> types;

    types["jpg"] = "image/jpg";
    types["png"] = "image/png";
    types["html"] = "text/html";
    types["css"] = "text/css";
    types["js"] = "application/javascript";

    if (file.find(".") == std::string::npos)
        return "text/plain";
    return types[file.substr(file.find_last_of(".") + 1)];
}

std::string getResponseBoilerPlate(const std::string& code, const std::string& title, const std::string& body) {
    std::string html;
    std::string response = "";

    html += "<!DOCTYPE html>\n";
    html += "<html lang='en'>\n";
    html += "<head>\n";
    html += "    <meta charset='UTF-8'>\n";
    html += "    <meta http-equiv='X-UA-Compatible' content='IE=edge'>\n";
    html += "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n";
    html += "    <title>" + title + "</title>\n";
    html += "</head>\n";
    html += "<body>\n";
    html += body;
    html += "</body>\n";
    html += "</html>\n";

    response += "HTTP/1.1 " + code + "\n";
    response += "Content-Type: text/html\n";
    response += "Content-Length: " + ft_ntos(html.size()) + "\n\n"; 
    response += html;

    return response;
}

std::string getFileSize(std::string file)
{
    std::streampos begin, end;
    std::stringstream stream;
    std::string size;

    std::ifstream open_file(file.c_str(), std::ios::binary | std::ios::in);


    begin = open_file.tellg();
    open_file.seekg (0, std::ios::end);
    end = open_file.tellg();
    open_file.close();

    stream << end - begin;
    stream >> size;

    return size;
}

std::string getHeader(std::string file, std::string& code)
{
    return "HTTP/1.1 " + code + " OK\nContent-Type: " + getFileType(file) + "; charset=UTC-8\nContent-Length: " + getFileSize(file) +"\n\n";
}