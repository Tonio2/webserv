#pragma once

#include "webserv.hpp"
#include "vServerConfig.hpp"
#include "Request.hpp"

class vServer {
    public:
        vServer() {}
        vServer(vServerConfig vserv_conf) {
            _vserv_conf = vserv_conf;
        }
        ~vServer() {}

        void add_request(int sock) {
            _requests.insert(std::make_pair(sock, Request(_vserv_conf)));
        }

        void generate_response(int sock, int error_code) {
            _requests[sock].generate_response(error_code);
        }

        int read_request(int sock) {
            char buf[4096];
            memset(buf, 0, 4096);
            int ret = read(sock, buf, 4096);


            if (ret  == -1)
            {
                generate_response(sock, 500);
                return (2);
            }

            try
            {
                if (_requests[sock].get_raw_request(buf)) // find if request is over and if so parse it and return 1 if not finished
                    return (0);
            
                _requests[sock].find_and_process_conf();

                return (_requests[sock].process_request()); //process request and return 1 if waiting for cgi and 2 if finished
            }
            catch (int e)
            {
                generate_response(sock, e);
                return (2);
            }
        }

        int get_cgi_fd(int sock) {
            return _requests[sock].get_cgi_fd();
        }

        void get_cgi_output(int sock) {
            try
            {
                _requests[sock].get_cgi_output();
            }
            catch (int e)
            {
                generate_response(sock, e);
            }
        }

        void send_response(int sock) {
            write(sock, _requests[sock].get_response().c_str(), _requests[sock].get_response().size());
        }

        void remove_socket(int sock) {
            _requests.erase(sock);
        }

    private:
        vServerConfig               _vserv_conf;
        std::map<int, Request>      _requests;
};