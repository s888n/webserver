# Webserv

## Description

This project is an Nginx-Like HTTP web server that can handle multiple clients at once. It is able to serve static files and dynamic content. The server is able to handle GET, POST, PUT, DELETE requests and can also handle CGI scripts

## Requirements

- C++98
- Make

## How to run

To run the server, simply run the following command:

```
make
./webserv [config_file]
```

The server will then be running on the port specified in the config file. The default config file is `configs/default.conf`

## Features

- [x] GET requests
- [x] POST requests
- [x] PUT requests
- [x] DELETE requests
- [x] CGI scripts (python and javascript)
- [x] Config file
- [x] Error handling
- [x] io multiplexing
- [x] HTTP/1.1
- [x] Chunked transfer encoding
- [x] file upload
- [x] video streaming

