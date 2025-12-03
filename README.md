
# Webserv

<p align="center">
  <img width="150" height="150" alt="Image" src="https://github.com/user-attachments/assets/cfe0b16d-6b54-44a2-91c8-94ae70fd1ae2" />
</p>

Webserv is a custom implementation of a lightweight HTTP/1.1 server written in C++98. It is designed to handle multiple clients simultaneously using non-blocking I/O and `poll()`. The server supports standard HTTP methods, CGI execution, and extensive configuration via a file format similar to NGINXx. This project serves as a deep dive into network programming, socket manipulation, and the HTTP protocol.

## Status
Finished: 2025-07-11. Grade: 125/100.

## Supported Features

### Core HTTP Functionality
- **HTTP Methods**: Supports `GET`, `POST`, and `DELETE` requests.
- **I/O Multiplexing**: Uses `poll()` to handle multiple non-blocking client connections simultaneously.
- **Static Content**: Serves static files (HTML, CSS, JS, images) and supports directory listing (`autoindex`).
- **Error Handling**: Custom error pages for standard HTTP status codes (404, 403, 500, etc.).
- **Redirections**: Supports HTTP redirections (e.g., 301 Moved Permanently).

### Advanced Features
- **CGI Support**: Executes external scripts (`.cgi`) via `execve` to generate dynamic content.
- **File Uploads**: Allows clients to upload files to the server.
- **Session Management**: Implements basic session tracking using cookies.
- **Chunked Transfer Encoding**: Supports chunked requests and responses.
- **Timeouts**: Manages client timeouts to prevent hanging connections.

### Configuration
The server is configured via a file (default: `conf/default.conf`) that supports:
- Multiple virtual servers (host/port based).
- Custom root directories and index files.
- Client body size limits.
- Route-specific configurations (`location` blocks).
- Allowed HTTP methods per route.

## Process Overview
1.  **Initialization**: Parses the configuration file and sets up listening sockets for each server block.
2.  **Event Loop**: Enters a main loop using `poll()` to monitor sockets for events (read/write).
3.  **Connection Handling**: Accepts new client connections and adds them to the monitored set.
4.  **Request Parsing**: Reads raw data from sockets and parses HTTP headers and bodies.
5.  **Request Processing**:
    - Checks for valid methods, permissions, and file existence.
    - Routes the request to the appropriate handler (Static File, CGI, Upload, etc.).
6.  **Response Generation**: Constructs the HTTP response (headers + body).
7.  **Sending**: Writes the response back to the client socket, handling partial writes if necessary.
8.  **Cleanup**: Closes connections based on `Keep-Alive` headers or timeouts.

## Dependencies
- **C++ Compiler**: `c++` (clang++ or g++) supporting C++98 standard.
- **Make**: GNU Make for build automation.
- **Python/Node.js**: (Optional) For running specific CGI scripts.

## How To Use

To clone and run this application:

```sh
# Clone this repository
$ git clone git@github.com:exellaz/webserv.git

# Go into repository
$ cd webserv

# Build program
$ make

# Run program (uses conf/default.conf by default)
$ ./webserv

# Run with a specific config file
$ ./webserv conf/my_config.conf
```

## Configuration Example
The configuration file uses a syntax similar to NGINX:

```nginx
server {
    listen 0.0.0.0:8000;
    server_name server1;
    root /path/to/root;

    error_page 404 /error_page/404.html;
    client_max_body_size 1048576000;

    location / {
        alias /html;
        index main.html;
        allowed_method GET;
    }

    location /upload {
        allow_upload on;
        allowed_method POST;
    }

    location /cgi-bin {
        cgi_path on;
        allowed_method GET POST DELETE;
    }
}
```
