#include "webserv.hpp"
#include "Server.hpp"

int main(int ac, char** av) {
    std::string conf_path = "default.conf";
    if (ac >= 2)
        conf_path = av[1];
    try
    {
        Server server(conf_path);
        if (server.start_listening())
            return (1);
        server.run();
    }
    catch(const int e)
    {
        return (1);
    }
}