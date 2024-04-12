#include "../inc/socketServer.hpp"

static void FindPR(std::string command, std::string toFind, std::string &str)
{
    size_t i = 0;

    for (; i < command.size(); i++)
    {
        if (command[i] != ' ')
        {
            std::string tmp;
            for (;i < command.size() && command[i] != ' '; i++)
                tmp += command[i];
            if (tmp == toFind)
                break;
            else tmp.clear();
        }
    }
    if (i < command.size())
        str = command.substr(i);
    i = 0;
    for (; i < str.size() && str[i] == ' '; i++)
        str = str.substr(1);
}

// void std::string splitPart(std::string &command, std::vector<str::>)