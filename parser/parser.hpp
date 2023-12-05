#ifndef PARSER_HPP
#define PARSER_HPP

#include "../server/server.hpp"
#include "../utils/utils.hpp"
#include "../webserv/webserv.hpp"
typedef std::vector<std::string> stringVector;
class location;
class server;
class parser
{
    private:

        std::vector<server> servers;
        std::string filename;
        std::string fileContent;
        // static const std::map<std::string, std::string> defaultValues;
    
    public:
        parser(std::string &filename);
        std::vector<server> getServers() const;

        static void trim(std::string &str, std::string chars);
        stringVector split(std::string str, std::string sep);
        static void removeComments(std::string &str);
        static void removeEmptyLines(std::string &str);
        static bool isEmpty(std::string str);
        void readFile();
        void validate();
        server parseServer(std::string &sb);
        location parseLocation(std::string locationBlock);

        int setPort(stringVector &values);
        std::string setHost(stringVector &values);
        std::string setRoot(stringVector &values);
        std::vector<std::string> setIndexes(stringVector &values);
        std::vector<std::string> setMethods(stringVector &values);
        std::vector<std::string> setServerNames(stringVector &values);
        std::map <int, std::string> setErrorPages(stringVector &values);
        bool setAutoindex(stringVector &values);
        size_t setMaxBodySize(stringVector &values);
        size_t getServerEnd(std::string &str, size_t pos);
        size_t getSeverStart(std::string &str, size_t pos);
        stringVector splitServerBlocks(std::string &str);
        void parseServerBlocks(stringVector sbs);
        stringVector extractLocations(std::string &str);
        server getServer(stringVector values);
        bool directiveExists(std::string directive, stringVector &values);
        void setServerDefaultValues(server &server,stringVector values);
        void validateDirectiveCount(stringVector &values);
};

#endif