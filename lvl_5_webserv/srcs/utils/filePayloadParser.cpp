#include "utils.hpp"

// Function to get the value of a header field from a string
std::string getHeaderValue(const std::string& str, const std::string& headerName) {
    std::size_t pos = str.find(headerName);
    if (pos != std::string::npos) {
        std::string value = str.substr(pos + headerName.length() + 2);
        pos = value.find("\r\n");
        value = value.substr(0, pos);
        return value;
    }
    return "";
}

// Function to get the filename from the content disposition header
// !TODO
// filename is not being getted properly
std::string getFilename(const std::string& line) {
    size_t pos = line.find("filename=") + 10;
    std::string filename = line.substr(pos, line.find('\r', pos) - 1);
    trimStr(filename, "\"\r\n");
    return filename;
}

// Function to get the file data from a file field part
// !TODO
void processUploadedFile(const std::string& filePart, const std::string& boundary) {
    std::string contentType;
    std::stringstream filePartStream(filePart);

    std::string line;

    // skip boundary
    std::getline(filePartStream, line, '\n');

    std::getline(filePartStream, line, '\n');
    std::cout << "FILENAME = " << getFilename(line) << std::endl;

    std::ofstream uploadedFile(getFilename(line).c_str(), std::ofstream::out | std::ofstream::binary);

    std::getline(filePartStream, line, '\n');
    contentType = line.substr(line.find("Content-Type:") + 14);

    // consume content_type
    std::getline(filePartStream, line, '\n');
    std::cout << "CONTENT_TYPE = " << contentType << std::endl;

    while (getNextLine(filePartStream, line)) {
        if (line.find("--" + boundary) != std::string::npos) {
            break;
        }
        uploadedFile.write(line.c_str(), line.length());
    }
    uploadedFile.close();
}
