#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include "Request.hpp"

class Response {
    public:
        Response();
        ~Response();

        void fill_response(const Request & request);
        std::string res_to_str() const;
    private:
        std::string method;
        std::string status_code;
        std::string content_type;
        std::string content_length;
        std::string body;
};

#endif