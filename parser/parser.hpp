#ifndef PARSER_HPP
#define PARSER_HPP

#include "../server/server.hpp"
#include "../utils/utils.hpp"
#include "../webserv/webserv.hpp"

class parser
{
    private:

        std::vector<server> servers;
        std::string filename;
    
    public:
        parser();
        parser(std::string &filename);
        std::vector<server> getServers() const;


};

#endif