#include "Request.hpp"

Request::Request() {}

Request::~Request() {}

void Request::fill_request(const std::string & request) {
    std::size_t pos = request.find(" ");
    method = request.substr(0, pos);
    std::size_t pos2 = request.find(" ", pos);
    path = request.substr(pos, pos2);
}

const std::string Request::get_method() const { return method; }

const std::string Request::get_path() const { return path; }