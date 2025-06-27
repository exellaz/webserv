#include "http-response.h"
#include "http-request.h"
#include "http-exception.h"

static std::vector<std::string> extractMultipartParts(const std::string& body, const std::string& boundary)
{
    std::vector<std::string> parts;
    size_t start = 0;
    size_t end;
    while ((end = body.find(boundary, start)) != std::string::npos) {
        std::string part = body.substr(start, end - start);
        if (!part.empty())
            parts.push_back(part);
        start = end + boundary.length();
    }
    return parts;
}

static std::string extractFilename(const std::string& part)
{
    size_t dispPos = part.find("Content-Disposition:");
    if (dispPos == std::string::npos)
        return "";

    size_t filenamePos = part.find("filename=", dispPos);
    if (filenamePos == std::string::npos)
        return "";

    size_t nameStart = part.find("\"", filenamePos);
    size_t nameEnd = part.find("\"", nameStart + 1);
    if (nameStart == std::string::npos || nameEnd == std::string::npos)
        return "";

    return part.substr(nameStart + 1, nameEnd - nameStart - 1);
}

static std::string extractFileContent(const std::string& part)
{
    size_t headerEnd = part.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return "";

    return part.substr(headerEnd + 4);
}

static std::string saveUploadedFile(const std::string& locationPath, const std::string& filename, const std::string& content)
{
    std::string path = locationPath + filename;
    std::ofstream out(path.c_str(), std::ios::binary);
    if (!out)
        throw HttpException(INTERNAL_ERROR, "Failed to create file");

    out.write(content.c_str(), content.length());
    out.close();
    return path;
}

void HttpResponse::handlePostRequest(const HttpRequest& request, const std::string& locationPath)
{
    std::string contentType = request.getHeader("Content-Type");
    if (contentType.find("multipart/form-data") == std::string::npos)
        throw HttpException(BAD_REQUEST, "Expected multipart/form-data");

    size_t pos = contentType.find("boundary=");
    if (pos == std::string::npos)
        throw HttpException(BAD_REQUEST, "Missing boundary in Content-Type");

    const std::string& body = request.getBody();
    std::string boundary = "--" + contentType.substr(pos + 9);
    std::vector<std::string> parts = extractMultipartParts(body, boundary);

    bool first = true;
    for (size_t i = 0; i < parts.size(); ++i)
    {
        const std::string& part = parts[i];
        std::string filename = extractFilename(part);
        if (filename.empty())
            continue;

        std::string fileContent = extractFileContent(part);
        if (fileContent.empty())
            continue;

        std::string outputPath = saveUploadedFile(locationPath, filename, fileContent);
        if (first) {
            setStatus(CREATED);
            setBody("File uploaded successfully to: " + outputPath);
            first = false;
        }
        else
            appendToBody(", " + outputPath);
    }
    appendToBody("\r\n");
    setHeader("Content-Type", "text/plain");
}