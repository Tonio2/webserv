#include "Response.hpp"

Response::Response() {}
Response::~Response() {}

void Response::fill_response(const Request & request) {
    (void)request;
    method = "HTTP/1.1";
    status_code = "200 OK";
    content_type = "Content-Type: text/plain";

    //fill body with proper file
    std::string line;
    std::ifstream myfile ("example.txt");
    body = "";
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            body += line + '\n';
        }
        myfile.close();
    }

    else 
    {
        std::cout << "Unable to open file";
        body = "Hello world!";
    }
    content_length = "Content-Length: " + std::to_string(body.length());
}

std::string Response::res_to_str() const {
    return method + ' ' + status_code + '\n' + content_type + '\n' + content_length + "\n\n" + body;
}