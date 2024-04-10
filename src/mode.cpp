#include "../inc/socketServer.hpp"

std::string Server::mode_Append(std::string array, char oper, char mode)
{
    std::stringstream stringStream;

    stringStream.clear();
    char last = '\0';
    for (size_t i = 0; i < array.size(); i++)
    {
        if (array[i] == '+' || array[i] == '-')
            last = array[i];
    }
    if (last != oper)
        stringStream<<oper<<mode;
    else    
        stringStream<<mode;
    return (stringStream.str());
}

std::vector<std::string> Server::splitParams(std::string params)
{
    if (!params.empty() && params[0] == ':')
        params.erase(params.begin());
    std::vector<std::string> tokens;
    std::string p;
    std::istringstream strinStream(params);
    while (std::getline(strinStream, p, ','))
    {
        tokens.push_back(p);
        p.clear();
    }
    return (tokens);
}


