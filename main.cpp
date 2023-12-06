#include "webserv/webserv.hpp"


int main(int ac, char **av)
{
    try {
            std::string config_file;
            if (ac != 2 && ac != 1)
                throw std::runtime_error("Usage: ./webserv [config_file]");
            if (ac == 2)
                config_file = av[1];
            else
                config_file = "configs/default.conf";
            
            webserv webserver(config_file);
            webserver.init();
            webserver.run();
            return 0;
        }
        catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
}