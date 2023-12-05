#include "parser.hpp"

parser::parser(std::string &filename)
{
    this->filename = filename;
}

std::vector<server> parser::getServers() const
{
    return servers;
}