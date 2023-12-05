#include "parser.hpp"
void printServer(server &s);
parser::parser(std::string &filename)
{
    this->filename = filename;
    readFile();
    removeComments(fileContent);
    trim(fileContent, " \t\n\r");
    parseServerBlocks(splitServerBlocks(fileContent));
}

std::vector<server> parser::getServers() const
{
    return servers;
}

void parser::removeComments(std::string &str)
{
    //remove comments
    size_t start = str.find("#");
    while (start != std::string::npos)
    {
        size_t end = str.find("\n", start);
        str.erase(start, end - start);
        start = str.find("#");
    }
}

void parser::trim(std::string &str, std::string chars)
{
    str.erase(0, str.find_first_not_of(chars));
    str.erase(str.find_last_not_of(chars) + 1);
}


void parser::readFile()
{
    validate();
    std::ifstream file(filename.c_str());
    if (!file.is_open())
        throw std::runtime_error("Error: cannot open file");
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    fileContent = buffer.str();
    file.close();
}

void parser::validate()
{
    struct stat buffer;
    if (filename.substr (filename.find_last_of(".") +  1) != "conf")
        throw std::runtime_error("Error: file is not a .conf");
    if (stat(filename.c_str(), &buffer) != 0)
        throw std::runtime_error("Error: cannot open file");
    if (buffer.st_mode & S_IFDIR)
        throw std::runtime_error("Error: file is a directory");
    if(!(buffer.st_mode & S_IRUSR))
        throw std::runtime_error("Error: file is not readable");
    if (buffer.st_size == 0)
        throw std::runtime_error("Error: file is empty");
}

stringVector parser::split(std::string str, std::string sep)
{
    std::vector<std::string> result;
    std::string token;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (sep.find(str[i]) != std::string::npos)
        {
            if (token.size() > 0)
            {
                result.push_back(token);
                token.clear();
            }
        }
        else
            token += str[i];
    }
    if (token.size() > 0)
        result.push_back(token);
    return (result);
}

size_t parser::getSeverStart(std::string &str, size_t pos)
{
    size_t i;
    // find 'server' keyword
    for (i = pos; str[i]; i++)
    {
        if (str[i] == 's')
            break;
        if (!isspace(str[i]))
            throw std::runtime_error("Error: invalid server block");
    }
    if (!str[i])
        return pos;
    if (str.compare(i, 6, "server") != 0)
        throw std::runtime_error("Error: invalid server block");
    i += 6;
    while (str[i] && isspace(str[i]))
        i++;
    if (str[i] == '{')
        return i;
    else
        throw std::runtime_error("Error: invalid server block");
}

size_t parser::getServerEnd(std::string &str, size_t pos)
{
    size_t i;
    size_t scope;

    scope = 0;
    for (i = pos + 1; str[i]; i++)
    {
        if (str[i] == '{')
            scope++;
        if (str[i] == '}')
        {
            if (!scope)
                return i;
            scope--;
        }
    }
    return pos;
}

stringVector parser::splitServerBlocks(std::string &str)
{
    size_t start = 0;
    size_t end = 1;
    stringVector servers;

    if (str.find("server") == std::string::npos)
        throw std::runtime_error("No server block found");

    while (start != end && start < str.length())
    {
        start = getSeverStart(str, start);
        end = getServerEnd(str, start);
        if (start == end)
            throw std::runtime_error("Error: invalid server block");
        servers.push_back(str.substr(start, end - start + 1));
        start = end + 1;
    }
    return servers;
}

bool parser::isEmpty(std::string str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (!isspace(str[i]))
            return false;
    }
    return true;
}

void parser::removeEmptyLines(std::string &str)
{
    size_t start = 0;
    size_t end = 0;
    start = str.find("\n");
    while (start != std::string::npos)
    {
        end = str.find("\n", start);
        if (isEmpty(str.substr(start, end - start)))
            str.erase(start, end - start + 1);
        else
            start = end + 1;
    }
}
void parser::parseServerBlocks(stringVector sbs)
{
    for (size_t i = 0; i < sbs.size(); i++)
        servers.push_back(parseServer(sbs[i]));

}



size_t findLocationStart(std::string &serverBlock)
{
    size_t pos = serverBlock.find("location");
    return pos;
}
size_t findLocationEnd(std::string &serverBlock, size_t start)
{
    size_t end = serverBlock.find("}", start);
    return end;
}

stringVector parser::extractLocations(std::string &sb)
{
    stringVector locations;
    size_t start = findLocationStart(sb);
    if(start == std::string::npos)
        return locations;
    size_t end = findLocationEnd(sb, start);
    while (start != std::string::npos)
    {
        locations.push_back(sb.substr(start, end - start + 1));
        sb.erase(start, end - start + 1);
        start = findLocationStart(sb);
        end = findLocationEnd(sb, start);
    }
    return locations;
}
server parser::parseServer(std::string &sb)
{
    stringVector lbs = extractLocations(sb);
    std::string serverBlock = sb;
    serverBlock.erase(0, serverBlock.find("{") + 1);
    serverBlock.erase(serverBlock.find_last_of("}"));
    trim(serverBlock, " \t\n\r");
    server server = getServer(split(serverBlock, ";"));
    return server;
}



location parser::parseLocation(std::string locationBlock)
{
    location location;
    return location;
}


server parser::getServer(stringVector values)
{
    server server;
    validateDirectiveCount(values);
    for (size_t i = 0; i < values.size(); i++)
    {
        stringVector tokens = split(values[i], " \t\n\r");
        if (tokens.size() != 2)
            throw std::runtime_error("Error: invalid directive");
        if (tokens[0] == "port")
            server.port = setPort(tokens);
        else if (tokens[0] == "host")
            server.host = setHost(tokens);
        else if (tokens[0] == "root")
            server.root = setRoot(tokens);
        else if (tokens[0] == "index")
            server.indexes = setIndexes(tokens);
        else if (tokens[0] == "allow")
            server.methods = setMethods(tokens);
        else if (tokens[0] == "server_names")
            server.server_names = setServerNames(tokens);
        else if (tokens[0] == "error_pages")
            server.error_pages = setErrorPages(tokens);
        else if (tokens[0] == "autoindex")
            server.autoindex = setAutoindex(tokens);
        else if (tokens[0] == "max_body_size")
            server.max_body_size = setMaxBodySize(tokens);
        else
            throw std::runtime_error("Error: unknown directive ");
    }
    setServerDefaultValues(server, values);
    printServer(server);
    std::cout<<"-------------------"<<std::endl;
    return server;
}

int parser::setPort(stringVector &values)
{
    if(values.size() != 2)
        throw std::runtime_error("Error: invalid port");
    if(values[1].find_first_not_of("0123456789") != std::string::npos || values[1].length() > 5)
        throw std::runtime_error("Error: invalid port");
    int portInt = std::atoi(values[1].c_str());
    if (portInt < 128 || portInt > 65535)
        throw std::runtime_error("Error: invalid port");
    return portInt;
}

std::string parser::setHost(stringVector &values)
{
    if(values.size() != 2)
        throw std::runtime_error("Error: invalid host");
    if (values[1].find_first_not_of("0123456789.") != std::string::npos)
        throw std::runtime_error("Error: invalid host");
    stringVector tokens = split(values[1], ".");
    if (tokens.size() != 4)
        throw std::runtime_error("Error: invalid host");
    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].find_first_not_of("0123456789") != std::string::npos)
            throw std::runtime_error("Error: invalid host");
        int num = std::atoi(tokens[i].c_str());
        if (num < 0 || num > 255)
            throw std::runtime_error("Error: invalid host");
    }
    return values[1];
}

std::string parser::setRoot(stringVector &values)
{
    if(values.size() != 2)
        throw std::runtime_error("Error: invalid root");
    std::string root = values[1];
    return root;
}

std::vector<std::string> parser::setIndexes(stringVector &values)
{
    if(values.size() == 1)
        throw std::runtime_error("Error: invalid indexes");
    stringVector tokens = split(values[1], ",");
    return tokens;
}

std::vector<std::string> parser::setMethods(stringVector &values)
{
    if(values[1].find_first_not_of("GETPOSTDELETE,") != std::string::npos)
        throw std::runtime_error("Error: invalid methods");
    stringVector tokens = split(values[1], ",");
    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i] != "GET" && tokens[i] != "POST" && tokens[i] != "DELETE")
            throw std::runtime_error("Error: invalid methods");
    }
    return tokens;
}

std::vector<std::string> parser::setServerNames(stringVector &values)
{
    if(values.size() == 1)
        throw std::runtime_error("Error: invalid server_names");
    stringVector tokens = split(values[1], ",");
    return tokens;
}

std::map <int, std::string> parser::setErrorPages(stringVector &values)
{

    stringVector tokens = split(values[1], ",");
    std::map <int, std::string> error_pages;
    for (size_t i = 0; i < tokens.size(); i++)
    {
        stringVector tokens2 = split(tokens[i], "=");
        if (tokens2.size() != 2)
            throw std::runtime_error("Error: invalid error_pages");
        if (tokens2[0].find_first_not_of("0123456789") != std::string::npos)
            throw std::runtime_error("Error: invalid error_pages");
        int code = std::atoi(tokens2[0].c_str());
        if (code < 100 || code > 599)
            throw std::runtime_error("Error: invalid error_pages");
        error_pages[code] = tokens2[1];
    }
    return error_pages;
}

bool parser::setAutoindex(stringVector &values)
{
    if(values.size() != 2)
        throw std::runtime_error("Error: invalid autoindex");
    if (values[1] == "on")
        return true;
    else if (values[1] == "off")
        return false;
    else
        throw std::runtime_error("Error: invalid autoindex");
}

size_t parser::setMaxBodySize(stringVector &values)
{
    size_t magnitude = 1;
    if (values[1].find_first_not_of("0123456789mkg") != std::string::npos || values[1].length() > 10)
        throw std::runtime_error("Error: invalid max_body_size");
    if(std::count(values[1].begin(), values[1].end(), 'm') > 1 || std::count(values[1].begin(), values[1].end(), 'k') > 1 || std::count(values[1].begin(), values[1].end(), 'g') > 1)
        throw std::runtime_error("Error: invalid max_body_size");
    if (values[1].find_first_of("mkg") == std::string::npos)
        magnitude = 1;
    else
    {
        if (values[1].find("m") != std::string::npos)
            magnitude = 1000000;
        else if (values[1].find("k") != std::string::npos)
            magnitude = 1000;
        else if (values[1].find("g") != std::string::npos)
            magnitude = 1000000000;
    values[1].erase(values[1].find_first_of("mkg"));
    }
    size_t max_body_size = std::stoul(values[1].c_str()) * magnitude;
    if (max_body_size > 100000000)
        max_body_size = 100000000;
    return max_body_size;
}

void parser::setServerDefaultValues(server &server,stringVector values)
{
    for(size_t i = 0 ; i < values.size(); i++)
    {
        if (!directiveExists("port", values))
            throw std::runtime_error("Error: missing port directive");
        if (!directiveExists("host", values))
            server.host = "0.0.0.0";
        if (!directiveExists("root", values))
            server.root = "./wwww";
        if (!directiveExists("index", values)&& server.indexes.size() == 0)
            server.indexes.push_back("index.html");
        if (!directiveExists("allow", values)&& server.methods.size() == 0)
            server.methods.push_back("GET");
        if (!directiveExists("server_names", values)&& server.server_names.size() == 0)
            server.server_names.push_back("localhost");
        if (!directiveExists("error_pages", values))
            server.error_pages[404] = "error.html";
        if (!directiveExists("autoindex", values))
            server.autoindex = false;
        if (!directiveExists("max_body_size", values))
            server.max_body_size = 1000000;
    } 

}

bool parser::directiveExists(std::string directive, stringVector &values)
{
    for (size_t i = 0; i < values.size(); i++)
    {
        stringVector tokens = split(values[i], " \t\n\r");
        if (tokens[0] == directive)
            return true;
    }
    return false;
}

void printServer(server &s)
{
    std::cout << "port: " << s.port << std::endl;
    std::cout << "host: " << s.host << std::endl;
    std::cout << "root: " << s.root << std::endl;
    std::cout << "indexes: ";
    for (size_t i = 0; i < s.indexes.size(); i++)
        std::cout << s.indexes[i] << " ";
    std::cout << std::endl;
    std::cout << "methods: ";
    for (size_t i = 0; i < s.methods.size(); i++)
        std::cout << s.methods[i] << " ";
    std::cout << std::endl;
    std::cout << "server_names: ";
    for (size_t i = 0; i < s.server_names.size(); i++)
        std::cout << s.server_names[i] << " ";
        std::cout << std::endl;
    std::cout << "error_pages: ";
    for (std::map<int, std::string>::iterator it = s.error_pages.begin(); it != s.error_pages.end(); it++)
        std::cout << it->first << " " << it->second << " ";
        std::cout << std::endl;
    std::cout << "autoindex: " << s.autoindex << std::endl;
    std::cout << "max_body_size: " << s.max_body_size << std::endl;
}

void parser::validateDirectiveCount(stringVector &values)
{
    stringVector directives;
    directives.push_back("port");
    directives.push_back("host");
    directives.push_back("root");
    directives.push_back("index");
    directives.push_back("allow");
    directives.push_back("server_names");
    directives.push_back("error_pages");
    directives.push_back("autoindex");
    directives.push_back("max_body_size");
    
    for (size_t i = 0; i < directives.size(); i++)
    {
        int count = 0;
        for (size_t j = 0; j < values.size(); j++)
        {
            stringVector tokens = split(values[j], " \t\n\r");
            if (tokens[0] == directives[i])
                count++;
        }
        if (count > 1)
            throw std::runtime_error("Error: duplicate directive");
    }
}