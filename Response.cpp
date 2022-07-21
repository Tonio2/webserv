#include "Response.hpp"
#include <map>

Response::Response() {}
Response::~Response() {}

void Response::fill_response(const Request & request) {
    //fill header

    //method
    method = "HTTP/1.1";

    //status_code
    code = 200;

    //fill body with proper file
    std::string line;
    std::string path = "/index.html";
    if (request.get_path().size() > 1)
        path = request.get_path();
    std::ifstream myfile ("web_files" + path);

    std::string type;
    type = path.substr(path.rfind(".") + 1, path.size() - path.rfind("."));
	if (type == "html")
		content_type = "text/html";
	else if (type == "css")
		content_type = "text/css";
	else if (type == "js")
		content_type = "text/javascript";
	else if (type == "jpeg" || type == "jpg")
		content_type = "image/jpeg";
	else if (type == "png")
		content_type = "image/png";
	else if (type == "bmp")
		content_type = "image/bmp";
	else
		content_type = "text/plain";
    
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
        code = 404;
        myfile.open("web_files/404.html");
        content_type = "text/html";
        while ( getline (myfile,line) )
        {
            body += line + '\n';
        }
        myfile.close();
    }
    content_length = body.length();
}

std::string Response::res_to_str() const {
    std::map<int, std::string> status_message;
    status_message[200] = "OK";
    status_message[404] = "NOT FOUND";
    std::string response = "";
    response += method + ' ' + std::to_string(code) + ' ' + status_message[code] + "\r\n";
    response += "content-type: " + content_type + "\r\n";
    response += "content-length: " + std::to_string(content_length) + "\r\n\r\n";
    response += body;
    return response;
}