#include <iostream>

int main(int ac, char **av) {
    (void)av;
    if (ac != 2) {
        std::cout << "Usage: ./webserv config_filename\n";
        return 0;
    }
    std::cout << "Bien joué\n";
}