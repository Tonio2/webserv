#pragma once

#include "webserv.hpp"
#include "vServer.hpp"
#include "vServerConfig.hpp"

class Server {
    public:
        Server() {}
        Server(std::string conf_path) {
            std::ifstream		file;
	        std::stringstream	buffer;
            file.open(conf_path.c_str(), std::ifstream::in);
            if (!file.is_open())
                throw 1;
            
            buffer << file.rdbuf();
            std::string conf = buffer.str();
            file.close();

            size_t pos = 0;
            std::vector<std::string> lines;
            std::string line = get_next_line(conf, pos);
            while (line != "")
            {
                if (line[line.size() - 1] == '\n')
                    line.erase(line.size() - 1);
                trim(line);
                if (line != "")
                    lines.push_back(line);
                line = get_next_line(conf, pos);
            }

            std::vector<std::string>::iterator l = lines.begin();
            std::vector<std::string> av;
            std::vector<int> _legal_errors;
            _legal_errors.push_back(400);
            _legal_errors.push_back(403);
            _legal_errors.push_back(404);
            _legal_errors.push_back(405);
            _legal_errors.push_back(408);
            _legal_errors.push_back(413);
            _legal_errors.push_back(500);
            std::vector<std::string> _legal_methods;
            _legal_methods.push_back("GET");
            _legal_methods.push_back("POST");
            _legal_methods.push_back("DELETE");
            while (l != lines.end())
            {
                if (*l == "server")
                {
                    vServerConfig   vserv_conf;
                    l++;
                    while (*l != ";")
                    {
                        av = ft_split(*l, ' ');
                        if (av[0] == "host")
                        {
                            if (av.size() < 2)
                                throw 3;
                            vserv_conf._host = av[1];
                        }
                        if (av[0] == "port")
                        {
                            if (av.size() < 2)
                                throw 4;
                            vserv_conf._port = std::atoi(av[1].c_str());
                        }
                        if (av[0] == "server_name")
                        {
                            if (av.size() < 2)
                                throw 5;
                            vserv_conf._server_name = av[1];
                        }
                        if (av[0] == "error_page")
                        {
                            if (av.size() < 3)
                                throw 6;
                            int error = std::atoi(av[1].c_str());
                            if (std::find(_legal_errors.begin(), _legal_errors.end(), error) == _legal_errors.end())
                                throw 7;
                            struct stat s;
                            if (stat(av[2].c_str(), &s) != 0 )
                                throw 8;
                            if (!(s.st_mode & S_IFREG))
                                throw 8;
                            vserv_conf._error_pages[error] = av[2];
                        }
                        if (av[0] == "body_size")
                        {
                            if (av.size() < 2)
                                throw 9;
                            vserv_conf._body_size = std::atoi(av[1].c_str());
                        }
                        if (av[0] == "match")
                        {
                            if (av.size() < 2)
                                throw 10;
                            Route   route_conf;
                            route_conf._match = av[1];
                            l++;
                            while (*l != ";")
                            {
                                av = ft_split(*l, ' ');
                                if (av[0] == "method")
                                {
                                    if (av.size() < 2)
                                        throw 11;
                                    for (size_t i = 1; i < av.size(); i++)
                                    {
                                        if (std::find(_legal_methods.begin(), _legal_methods.end(), av[i]) == _legal_methods.end())
                                            throw 12;
                                        route_conf._allowed_methods.push_back(av[i]);
                                    }
                                }
                                if (av[0] == "redirect")
                                {
                                    if (av.size() < 3)
                                        throw 13;
                                    int status = std::atoi(av[1].c_str());
                                    if (status < 300 || status > 308)
                                        throw 14;
                                    route_conf._redirect = std::make_pair(status, av[2]);
                                }
                                if (av[0] == "root")
                                {
                                    if (av.size() < 2)
                                        throw 14;
                                    route_conf._root = av[1];
                                }
                                if (av[0] == "autoindex")
                                {
                                    if (av.size() < 2 || (av[1] != "on" && av[1] != "off"))
                                        throw 15;
                                    route_conf._autoindex = (av[1] == "on");
                                }
                                if (av[0] == "index")
                                {
                                    if (av.size() < 2)
                                        throw 16;
                                    route_conf._index = av[1];
                                }
                                if (av[0] == "cgi")
                                {
                                    if (av.size() < 3)
                                        throw 17;
                                    struct stat s;
                                    if (stat(av[2].c_str(), &s) != 0 )
                                        throw 18;
                                    if (!(s.st_mode & S_IFREG))
                                        throw 18;
                                    route_conf._cgi_path = std::make_pair(av[1], av[2]);
                                }
                                if (av[0] == "upload")
                                {
                                    if (av.size() < 2)
                                        throw 19;
                                    struct stat s;
                                    if (stat(av[1].c_str(), &s) != 0 )
                                        throw 20;
                                    if (!(s.st_mode & S_IFDIR))
                                        throw 20;
                                    route_conf._upload = av[1];
                                }
                                l++;
                            }
                            if (route_conf._match == "/")
                                vserv_conf._is_mvp = true;
                            vserv_conf._routes_conf.push_back(route_conf);
                        }
                        l++;
                    }
                    if (vserv_conf._is_mvp)
                        _vservers_conf.push_back(vserv_conf);
                }
                else
                    throw 2;
                l++;
            }
        }
        ~Server() {}

        int start_listening() {
            int sockfd;
            int opt = 1;
            struct sockaddr_in address;
            for (size_t i = 0; i < _vservers_conf.size(); i++)
            {
                if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                {
                    this->log("socket creation failed", ERROR);
                    continue;
                }

                if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
                {
                    this->log("setsockopt failed : " + std::string(strerror(errno)), ERROR);
                    continue;
                }

                address.sin_family = AF_INET;
                inet_pton(AF_INET, _vservers_conf[i]._host.c_str(), &address.sin_addr);
                address.sin_port = htons(_vservers_conf[i]._port);
                if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) == -1)
                {
                    this->log("bind failed", ERROR);
                    continue;
                }

                if (listen(sockfd, 1024) == -1)
                {
                    this->log("listen failed", ERROR);
                    continue;
                }

                _vservers.insert(std::make_pair(sockfd, vServer(_vservers_conf[i])));
            }
            if (_vservers.size() == 0)
                return (1);
            return (0);
        }

        void run() {
            int debug_count = 0;
            std::vector<struct kevent>  change_list;
            struct kevent               event_list[1024];
            struct timespec *           timeout = 0;
            int                         kq;

            if ((kq = kqueue()) == -1)
            {
                this->log("kqueue error", ERROR);
                throw 0;
            }

            change_list.resize(_vservers.size());
            std::map<int, vServer>::const_iterator vserver = _vservers.begin();
            for (size_t i = 0; i < _vservers.size(); i++)
            {
                EV_SET(&(change_list.data()[i]), vserver->first, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
                vserver++;
            }
            
            int                                 event_count;
            int                                 new_sock;
            sockaddr_in                         client;
            socklen_t                           client_size = sizeof(client);
            struct kevent                       event;
            std::map<int, vServer>::iterator    vserv;
            std::map<int, Request>::iterator    req;
            int                                 req_state;
            int                                 udata[2];
            while (1)
            {
                if ((event_count = kevent(kq, change_list.data(), change_list.size(), event_list, 1024, timeout)) == -1)
                {
                    this->log("kevent failed", ERROR);
                    throw 0;
                }
                change_list.clear();
                for (int id = 0; id < event_count; id++)
                {
                    event = event_list[id];
                    if (event.flags & EV_ERROR)
                    {
                        this->log("error on fd " + ft_to_string(event.ident) + " " + strerror(event.data), ERROR);
                        continue;
                    }


                    if ((vserv = _vservers.find(event.ident)) != _vservers.end())
                    {
                        while (event.data--)    
                        {
                            if ((new_sock = accept(event.ident, (struct sockaddr *)&client, &client_size)) == -1)
                            {
                                this->log("accept failed", ERROR);
                            }
                            change_list.resize(change_list.size() + 2);
                            udata[0] = event.ident;
                            udata[1] = 0;
                            EV_SET(&(change_list.data()[change_list.size() - 2]), new_sock, EVFILT_READ, EV_ADD, 0, 0, (void *)(udata));
                            EV_SET(&(change_list.data()[change_list.size() - 1]), new_sock, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, 6000, (void *)(udata));
                            vserv->second.add_request(new_sock);
                        }
                        continue;
                    }

                    if (event.udata)
                    {
                        vserv = _vservers.find(((uintptr_t *)(event.udata))[0]);
                        if (event.filter == EVFILT_TIMER)
                        {
                            vserv->second.generate_response(event.ident, 408);
                            change_list.resize(change_list.size() + 1);
                            udata[0] = vserv->first;
                            udata[1] = 0;
                            EV_SET(&(change_list.data()[change_list.size() - 1]), event.ident, EVFILT_WRITE, EV_ADD, 0, 0, (void *)(udata));
                            continue ;
                        }

                        if (event.filter == EVFILT_READ && ((int *)(event.udata))[1] == 0)
                        {
                            req_state = vserv->second.read_request(event.ident); // socket = event.ident -> renvoie 0 si attends requete, 1 si attends cgi, 2 si reponse finalise
                            if (req_state == 1)
                            {
                                change_list.resize(change_list.size() + 1);
                                udata[0] = vserv->first;
                                udata[1] = event.ident;
                                EV_SET(&(change_list.data()[change_list.size() - 1]), vserv->second.get_cgi_fd(event.ident), EVFILT_READ, EV_ADD, 0, 0, (void *)(udata));
                            }
                            if (req_state == 2)
                            {
                                change_list.resize(change_list.size() + 1);
                                udata[0] = vserv->first;
                                udata[1] = 0;
                                EV_SET(&(change_list.data()[change_list.size() - 1]), event.ident, EVFILT_WRITE, EV_ADD, 0, 0, (void *)(udata));
                            }
                            continue;
                        }

                        //read cgi output
                        if (event.filter == EVFILT_READ && ((int *)(event.udata))[1])
                        {
                            int tmp = ((int *)(event.udata))[1];
                            vserv->second.get_cgi_output(((int *)(event.udata))[1]);
                            change_list.resize(change_list.size() + 1);
                            udata[0] = vserv->first;
                            udata[1] = 0;
                            EV_SET(&(change_list.data()[change_list.size() - 1]), tmp, EVFILT_WRITE, EV_ADD, 0, 0, (void *)(udata));
                            continue;
                        }

                        if (event.filter == EVFILT_WRITE)
                        {
                            std::cout << "sent response " << ++debug_count << "\n";
                            vserv->second.send_response(event.ident);
                        }

                        if (event.filter == EVFILT_WRITE || event.flags & EV_EOF)
                        {
                            change_list.resize(change_list.size() + 1);
                            EV_SET(&(change_list.data()[change_list.size() - 1]), event.ident, EVFILT_TIMER, EV_DELETE, 0, 0, 0);
                            if (vserv->second.get_cgi_fd(event.ident))
                                close(vserv->second.get_cgi_fd(event.ident));
                            close(event.ident);
                            vserv->second.remove_socket(event.ident);
                        }
                    }
                }
            }
        }

        void log(std::string str, int value) {
            if (value >= LOG_LVL)
                std::cout << str << "\n";
        }
    private:
        std::vector<vServerConfig>      _vservers_conf;
        std::map<int, vServer>          _vservers;

        
};