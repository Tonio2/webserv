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
        int code;
        std::string content_type;
        int content_length;
        std::string body;
};

#endif