#pragma once

#include "webserv.hpp"
#include "vServerConfig.hpp"

class Request {
    public:
        Request() {}
        Request(vServerConfig vserv_conf) {
            init_headers();
            _response = "";
            _code = 200;
            _protocol = "HTTP/1.0";
            _status_messages[200] = "OK";
            _status_messages[204] = "No Content";
            _status_messages[400] = "Bad Request";
            _status_messages[403] = "Forbidden";
            _status_messages[404] = "Not Found";
            _status_messages[405] = "Method Not Allowed";
            _status_messages[413] = "Payload Too Large";
            _status_messages[500] = "Internal Server Error";
            _body = "";
            _headers["Content-Type"] = "text/plain";
            _vserv_conf = vserv_conf;
            _cgi_path = "";
            _fd_out = 0;
        }
        ~Request() {}

        void init_headers() {
            _headers["Accept-Charsets"] = "";
            _headers["Accept-Language"] = "";
            _headers["Allow"] = "";
            _headers["Auth-Scheme"] = "";
            _headers["Authorization"] = "";
            _headers["Content-Language"] = "";
            _headers["Content-Length"] = "";
            _headers["Content-Location"] = "";
            _headers["Content-Type"] = "";
            _headers["Date"] = "";
            _headers["Host"] = "";
            _headers["Last-Modified"] = "";
            _headers["Location"] = "";
            _headers["Referer"] = "";
            _headers["Retry-After"] = "";
            _headers["Server"] = "";
            _headers["Transfer-Encoding"] = "";
            _headers["User-Agent"] = "";
            _headers["Www-Authenticate"] = "";
            _headers["Connection"] = "Keep-Alive";
        }

        void read_file(std::string path) {
            std::ifstream		file;
	        std::stringstream	buffer;
            file.open(path.c_str(), std::ifstream::in);
            
            std::string type = path.substr(path.rfind(".") + 1);
            if (type == "html")
                _headers["Content-Type"] = "text/html";
            else if (type == "css")
                _headers["Content-Type"] = "text/css";
            else if (type == "js")
                _headers["Content-Type"] = "text/javascript";
            else if (type == "jpeg" || type == "jpg")
                _headers["Content-Type"] = "image/jpeg";
            else if (type == "png")
                _headers["Content-Type"] = "image/png";
            else if (type == "bmp")
                _headers["Content-Type"] = "image/bmp";
            else
                _headers["Content-Type"] = "text/plain";


            if (!file.is_open())
                throw 403;
            
            buffer << file.rdbuf();
            _body = buffer.str();
            file.close();
        }

        void format_response() {
            _headers["Content-Length"] = ft_to_string(_body.size());
            _response = _protocol + " " + ft_to_string(_code) + " " + _status_messages[_code] + "\r\n";
            for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
            {
                if (it->second != "")
                    _response += it->first + ": " + it->second + "\r\n";
            }
            _response += "\r\n" + _body;
        }

        void generate_response(int code) {
            if (_protocol != "HTTP/1.1")
                _protocol = "HTTP/1.0";
            _code = code;
            read_file(_vserv_conf._error_pages[code]);
            format_response();
        }

        void		processChunk()
        {
            std::string	head = _raw.substr(0, _raw.find("\r\n\r\n"));
            std::string	chunks = _raw.substr(_raw.find("\r\n\r\n") + 4, _raw.size() - 1);
            std::string	subchunk = chunks.substr(0, 100);
            std::string	body = "";
            int			chunksize = strtol(subchunk.c_str(), NULL, 16);
            size_t		i = 0;

            while (chunksize)
            {
                i = chunks.find("\r\n", i) + 2;
                body += chunks.substr(i, chunksize);
                i += chunksize + 2;
                subchunk = chunks.substr(i, 100);
                chunksize = strtol(subchunk.c_str(), NULL, 16);
            }

            _raw = head + "\r\n\r\n" + body + "\r\n\r\n";
        }

        std::string percent_decode(std::string str) {
            std::string ret;
            size_t str_i = 0;

            while (str_i < str.size())
            {
                if (str[str_i] == '%')
                {
                    long int li = strtol(str.substr(str_i + 1, 2).c_str(), 0, 16);
                    str_i += 3;
                    ret += li;
                }
                else
                    ret += str[str_i++];
            }
            return (ret);
        }

        void parse_request() {
            size_t pos = 0;
            std::string line = getline(_raw, pos);
            size_t pos2 = line.find(" ", 0);
            if (pos2 == std::string::npos)
                throw 400;
            _method = line.substr(0, line.find(" ", 0));
            size_t pos3 = line.find(" ", pos2 + 1);
            if (pos3 == std::string::npos)
                throw 400;
            _url = line.substr(pos2 + 1, pos3 - (pos2 + 1));
            _protocol = line.substr(pos3 + 1);

            if (std::find(_legal_methods.begin(), _legal_methods.end(), _method) == _legal_methods.end())
                throw 400;
            
            if (_url[0] != '/')
                throw 400;
            
            _url = percent_decode(_url);
            
            if (_protocol != "HTTP/1.1" && _protocol != "HTTP/1.0")
            {
                _protocol = "HTTP/1.0";
                throw 400;
            }
            
            size_t pos4;
            std::string key;
            std::string value;
            while ((line = getline(_raw, pos)) != "")
            {
                pos4 = line.find(":", 0);
                if (pos4 == std::string::npos)
                    throw 400;
                key = line.substr(0, pos4);
                trim(key);
                value = line.substr(pos4 + 1);
                trim(value);
                if (_headers.count(key))
                    _headers[key] = value;
            }

            if (pos != std::string::npos)
                _req_body = _raw.substr(pos);
            
            _req_body = percent_decode(_req_body);

            if (_url.find("?", 0) != std::string::npos)
            {
                _query = _url.substr(_url.find("?", 0) + 1);
                _url = _url.substr(0, _url.find("?", 0));
            }
        }

        int get_raw_request(std::string buf) {
            _raw += buf;

            if (_raw.find("Content-Length: ") == std::string::npos)
            {
                if (_raw.find("Transfer-Encoding: chunked") != std::string::npos)
                {
                    if (_raw.substr(_raw.size() - 5) != "0\r\n\r\n")
                        return (1);
                }
            }

            size_t  len = 0;
            if (_raw.find("Content-Length: ") != std::string::npos)
                len = std::atoi(_raw.substr(_raw.find("Content-Length: ") + 16, 10).c_str());
            size_t  i = _raw.find("\r\n\r\n");
            if (_raw.size() < len + i + 4)
                return (1);

            if (_raw.find("Transfer-Encoding: chunked") != std::string::npos && _raw.find("Transfer-Encoding: chunked") < _raw.find("\r\n\r\n"))
                processChunk();

            // _req_body = _raw.substr(_raw.find("\r\n\r\n") + 4);

            parse_request();
            return (0);
        }

        bool is_match(std::string url, std::string match)
        {
            if (url.substr(0, match.size()) == match)
            {
                if (url.size() == match.size())
                    return true;
                if (url[match.size()] == '/')
                    return true;
            }
            return false;
        }

        void    find_and_process_conf() {
            size_t size_match = 0;
            _conf_id = 0;
            for (size_t i = 0; i != _vserv_conf._routes_conf.size(); i++)
            {
                if (is_match(_url, _vserv_conf._routes_conf[i]._match))
                {
                    if (_vserv_conf._routes_conf[i]._match.size() > size_match)
                    {
                        _conf_id = i;
                        size_match = _vserv_conf._routes_conf[i]._match.size();
                    }
                }
            }
            Route route_conf = _vserv_conf._routes_conf[_conf_id];
            
            _location = _url;
            if (route_conf._match == "/")
                _location.replace(0, 1, route_conf._root + "/");
            else
                _location.replace(0, route_conf._match.size(), route_conf._root);
            _location = "." + _location;
            //TODO location

            if (_location == "./html/upload.py")
            {
                _cgi_path = "/usr/local/bin/python3";
                // _location = "./html/upload.py";
            }
                
            else
            {
                std::string type = _url.substr(_url.rfind(".") + 1);
                if (type == route_conf._cgi_path.first)
                    _cgi_path = route_conf._cgi_path.second;
            }
            
        }

        int process_request() {
            Route route_conf = _vserv_conf._routes_conf[_conf_id];
            std::vector<std::string>::iterator it = route_conf._allowed_methods.begin();
            std::vector<std::string>::iterator ite = route_conf._allowed_methods.end();
            if (std::find(it, ite, _method) == ite)
                throw 405;
            if (_req_body.size() > _vserv_conf._body_size)
                throw 413;
            if (route_conf._redirect.first)
            {
                _code = route_conf._redirect.first;
                _headers["Location"] = route_conf._redirect.second;
                format_response();
                return (2);
            }
            if (_method == "GET")
                return (get());
            if (_method == "POST")
                return (post());
            if (_method == "PUT")
                return (post());
            if (_method == "DELETE")
                return (delete_method());
            return (0);
        }

        void init_env() {
            _env["REDIRECT_STATUS"] = "200";
            _env["GATEWAY_INTERFACE"] = "CGI/1.1";
            _env["SCRIPT_FILENAME"] = _location;
            _env["REQUEST_METHOD"] = _method;
            _env["CONTENT_LENGTH"] = ft_to_string(_req_body.size());
            _env["CONTENT_TYPE"] = _headers["Content-Type"];
            _env["QUERY_STRING"] = _query;
            _env["UPLOAD_PATH"] = _vserv_conf._routes_conf[_conf_id]._upload;
        }

        char ** format_env() {
            char	**env = new char*[_env.size() + 1];
            int	j = 0;
            for (std::map<std::string, std::string>::const_iterator i = this->_env.begin(); i != this->_env.end(); i++) {
                std::string	element = i->first + "=" + i->second;
                env[j] = new char[element.size() + 1];
                env[j] = strcpy(env[j], (const char*)element.c_str());
                j++;
            }
            env[j] = 0;
            return env;
        }

        void exec_cgi() {
            init_env();
            char **env = format_env();

            int			initial_stdin = dup(0);
            FILE		*tmpfile_in = tmpfile();
            long		fd_in = fileno(tmpfile_in);
            write(fd_in, _req_body.c_str(), _req_body.size());
	        lseek(fd_in, 0, SEEK_SET);
            int response_fd[2];

	        pipe(response_fd);

            int	pid = fork();
            if (pid == 0)
            {
                dup2(fd_in, STDIN_FILENO);
		        close(response_fd[0]);
                dup2(response_fd[1], 1);
                close(response_fd[1]);
                char *argv[] = {const_cast<char *>(_cgi_path.c_str()), const_cast<char *>(_location.c_str()), 0};
                execve(_cgi_path.c_str(), argv, env);
                write(response_fd[0], "Status: 500\r\n\r\n", 15);
                std::cerr << "execve crashing bad\n";
            }
            else
            {
                dup2(initial_stdin, STDIN_FILENO);
                fclose(tmpfile_in);
                close(fd_in);
                close(initial_stdin);
                close(response_fd[1]);
                _fd_out = response_fd[0];
            }
            
            for (size_t i = 0; env[i]; i++)
                delete[] env[i];
            delete[] env;

            if (!pid)
                exit(0);
        }

        void generate_autoindex(std::string location) {
            std::string dirName(_url);
            DIR *dir = opendir(location.c_str());
            _headers["Content-Type"] = "text/html";
            _body =\
            "<!DOCTYPE html>\n\
            <html>\n\
            <head>\n\
                    <title>" + dirName + "</title>\n\
            </head>\n\
            <body>\n\
            <h1>INDEX</h1>\n\
            <p>\n";

            if (dir == NULL) {
                std::cerr << "Error: could not open [" << location << "]" << std::endl;
                return;
            }
            if (dirName[0] != '/')
                dirName = "/" + dirName;
            for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir)) {
                _body += getLink(std::string(dirEntry->d_name), dirName);
            }
            _body +="\
            </p>\n\
            </body>\n\
            </html>\n";
            closedir(dir);
            _code = 200;
        }

        std::string getLink(std::string const &dirEntry, std::string dirName) {
            std::stringstream   ss;
            std::string slash = "/";
            if (dirName[dirName.size() - 1] == '/')
                slash = "";
            ss << "\t\t<p><a href=\"" << dirName << slash << dirEntry + "\">" + dirEntry + "</a></p>\n";
            return ss.str();
        }

        int get() {
            if (_cgi_path == "")
            {
                struct stat s;
                if (stat(_location.c_str(), &s) == 0 )
                {
                    if (s.st_mode & S_IFREG)
                        read_file(_location);
                    else if (s.st_mode & S_IFDIR)
                        generate_autoindex(_location);
                    else
                        throw 404;
                }
                else
                {
                    throw 404;
                }
                format_response();
                return (2);
            }
            else
            {
                exec_cgi();
                return (1);
            }
        }

        int post() {
            if (_cgi_path == "")
            {
                _code = 204;
                format_response();
                return (2);
            }
            else
            {
                exec_cgi();
                return (1);
            }
        }

        int delete_method() {
            struct stat s;
            if (stat(_location.c_str(), &s) == 0 && (s.st_mode & S_IFREG))
            {
                if (remove(_location.c_str()) == 0)
                {
                    _code = 204;
                    format_response();
                    return (2);
                }
                else
                    throw 403;
            }
            else
                throw 404;
            return (0);
        }

        int get_cgi_fd() const { return _fd_out; }

        void get_cgi_output() {
            char        buffer[65536] = {0};
            std::string cgi_output = "";

            lseek(_fd_out, 0, SEEK_SET);

            int read_ret = 1;
            while (read_ret > 0)
            {
                read_ret = read(_fd_out, buffer, 65535);
                if (read_ret == -1) break ;
                buffer[read_ret] = 0;
                cgi_output += buffer;
            }

            if (cgi_output == "")
                throw 500;
            
            size_t		i = 0;
		    size_t		j = cgi_output.size() - 2;
            while (cgi_output.find("\r\n\r\n", i) != std::string::npos || cgi_output.find("\r\n", i) == i)
            {
                std::string	str = cgi_output.substr(i, cgi_output.find("\r\n", i) - i);
                if (str.find("Status: ") == 0)
                    _code = std::atoi(str.substr(8, 3).c_str());
                else if (str.find("Content-type: ") == 0)
                {
                    _headers["Content-Type"] = str.substr(14, str.size());
                }
                i += str.size() + 2;
            }
            while (cgi_output.find("\r\n", j) == j)
                j -= 2;
            j+=2;

            if (_code == 500)
                throw 500;
            
            _body = cgi_output.substr(i, j - i);
            format_response();
            close(_fd_out);
        }

        std::string get_response() {
            return _response;
        }

        

    private:
        vServerConfig                       _vserv_conf;
        int                                 _conf_id;
        std::string                         _method;
        std::string                         _url;
        std::string                         _query;
        std::string                         _location;
        int                                 _code;
        std::string                         _protocol;
        std::string                         _body;
        std::string                         _response;
        std::map<std::string, std::string>  _headers;
        std::map<int, std::string>          _status_messages;
        std::string                         _raw;
        std::string                         _req_body;
        std::string                         _cgi_path;
        std::map<std::string, std::string>  _env;
        static std::vector<std::string>     init_methods();
        static std::vector<std::string>     _legal_methods;
        int 		                        _fd_out;
        // FILE *                              _tmpfile_out;
};

std::vector<std::string> Request::init_methods() {
    std::vector<std::string> ret;

    ret.push_back("GET");
    ret.push_back("POST");
    ret.push_back("HEAD");
    ret.push_back("DELETE");
    return ret;
}
std::vector<std::string> Request::_legal_methods = Request::init_methods();
