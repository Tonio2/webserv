#pragma once

#include "webserv.hpp"
#include "Route.hpp"

struct vServerConfig {
    vServerConfig() {
        _host = "";
        _port = 0;
        _error_pages.insert(std::make_pair(400, "html/400.html"));
        _error_pages.insert(std::make_pair(403, "html/403.html"));
        _error_pages.insert(std::make_pair(404, "html/404.html"));
        _error_pages.insert(std::make_pair(405, "html/405.html"));
        _error_pages.insert(std::make_pair(408, "html/408.html"));
        _error_pages.insert(std::make_pair(413, "html/413.html"));
        _error_pages.insert(std::make_pair(500, "html/500.html"));
        _server_name = "";
        _body_size = 65535;
        _is_mvp = false;
    }
    vServerConfig(const vServerConfig & vserv) {
        _host = vserv._host;
        _port = vserv._port;
        _server_name = vserv._server_name;
        _error_pages = vserv._error_pages;
        _body_size = vserv._body_size;
        _routes_conf = vserv._routes_conf;
    }
    vServerConfig & operator=(const vServerConfig & vserv) {
        _host = vserv._host;
        _port = vserv._port;
        _server_name = vserv._server_name;
        _error_pages = vserv._error_pages;
        _body_size = vserv._body_size;
        _routes_conf = vserv._routes_conf;
        return *this;
    }
    ~vServerConfig() {}

    std::string                 _host;
    int                         _port;
    std::string                 _server_name;
    std::map<int, std::string>  _error_pages;
    unsigned int                _body_size;
    std::vector<Route>          _routes_conf;
    bool                        _is_mvp;
};