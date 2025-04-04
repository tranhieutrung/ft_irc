#include "../includes/IO.hpp"
#include "../includes/Server.hpp"
#include <sstream>
#include <sys/socket.h>
#include <map>

ssize_t IO::sendCommand(int fd, const cmd &cmd)
{
    stringstream stream;
    if (!cmd.prefix.empty())
        stream << cmd.prefix << " ";
    stream << cmd.command;
    if (!cmd.arguments.empty())
        stream << " " << cmd.arguments;
    std::string sbuf = stream.str();

    return IO::sendString(fd, sbuf);
}

ssize_t IO::sendString(int fd, const std::string &s)
{
    log(DEBUG, "SEND", s);
    
    std::string message = s;
    message += "\r\n";

    return send(fd, message.c_str(), message.size(), 0);
}

ssize_t IO::sendCommandAll(const std::map<int, User> m, const cmd &cmd)
{
    ssize_t ret, result = 0;
    for (const auto &pair : m)
    {
        ret = sendCommand(pair.second.getFd(), cmd);
        if (ret < 0)
            return -1;
        result += ret;
    }
    return result;
}

ssize_t IO::sendCommandAll(const std::map<std::string, User> m, const cmd &cmd)
{
    ssize_t ret, result = 0;
    for (const auto &pair : m)
    {
        ret = sendCommand(pair.second.getFd(), cmd);
        if (ret < 0)
            return -1;
        result += ret;
    }
    return result;
}

ssize_t IO::sendStringAll(const std::map<int, User> m, const std::string &s)
{
    ssize_t ret, result = 0;
    for (const auto &pair : m)
    {
        ret = sendString(pair.second.getFd(), s);
        if (ret < 0)
            return -1;
        result += ret;
    }
    return result;
}

ssize_t IO::sendStringAll(const std::map<std::string, User> m, const std::string &s)
{
    ssize_t ret, result = 0;
    for (const auto &pair : m)
    {
        ret = sendString(pair.second.getFd(), s);
        if (ret < 0)
            return -1;
        result += ret;
    }
    return result;
}

std::vector<cmd> IO::recvCommands(int fd)
{
    char buf[512];
    ssize_t bytesReceived = recv(fd, buf, sizeof(buf), 0);
    if (bytesReceived == 0)
        return {{"", "DISCONNECT", ""}};
    if (bytesReceived < 0)
        return {{"", "ERROR", ""}};
    buf[bytesReceived] = '\0';
    
    
    istringstream stream(buf);
    std::string line;
    std::vector<cmd> list;
    
    while (getline(stream, line))
    {
        cmd cmd = {"", "", ""};
        istringstream lstream(line);
        if (line[0] == ':')
            getline(lstream, cmd.prefix, ' ');
        getline(lstream, cmd.command, ' ');
        getline(lstream, cmd.arguments, '\r');

        log(DEBUG, "RECV", line.substr(0, line.size() - 1) + " [" + cmd.prefix + "] [" + cmd.command + "] [" + cmd.arguments + "]");
        
        list.push_back(cmd);
    }
    return list;
}