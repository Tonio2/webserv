#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string.h>
#include <string>
#include <iostream>

class Request {
    public:
        Request();
        ~Request();

        void fill_request(const std::string & request);
        const std::string get_method() const;
        const std::string get_path() const;
    private:
        std::string method;
        std::string path;
};

#endif