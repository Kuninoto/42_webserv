# include <string>
# include <map>
# include <fstream>
# include <iostream>
# include <sstream>
# include <dirent.h>
# include <sys/stat.h>
# include <unistd.h>


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

std::string getHeader(std::string file)
{
    return "HTTP/1.1 200 OK\nContent-Type: " + getFileType(file) + "; charset=UTC-8\nContent-Length: " + getFileSize(file) +"\n\n";
}