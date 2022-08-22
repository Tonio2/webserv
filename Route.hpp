#pragma once

#include "webserv.hpp"

struct Route {
    Route() {
        _match = "/";
        _redirect = std::make_pair(0, "");
        _root = "";
        _autoindex = true;
        _index = "";
        _cgi_path = std::make_pair("", "");
        _upload = "";
    }
    Route(const Route & route) {
        _match = route._match;
        _allowed_methods = route._allowed_methods;
        _redirect = route._redirect;
        _root = route._root;
        _autoindex = route._autoindex;
        _index = route._index;
        _cgi_path = route._cgi_path;
        _upload = route._upload;
    }
    Route & operator =(const Route & route) {
        _match = route._match;
        _allowed_methods = route._allowed_methods;
        _redirect = route._redirect;
        _root = route._root;
        _autoindex = route._autoindex;
        _index = route._index;
        _cgi_path = route._cgi_path;
        _upload = route._upload;
        return *this;
    }
    ~Route() {}

    std::string                         _match;
    std::vector<std::string>            _allowed_methods;
    std::pair<int, std::string>         _redirect;
    std::string                         _root;
    bool                                _autoindex;
    std::string                         _index;
    std::pair<std::string, std::string> _cgi_path;
    std::string                         _upload;
};