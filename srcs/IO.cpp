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
    log(DEBUG, "SEND " + to_string(fd), s);
    
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
    static std::string message[42]; // replace with some max limit of clients
    char buf[512];
    ssize_t bytesReceived = recv(fd, buf, sizeof(buf), 0);

    if (bytesReceived <= 0)
    {
        message[fd] = "";
        if (bytesReceived == 0)
            return {{"", "DISCONNECT", ""}};
        else
            return {{"", "ERROR", ""}};
    }

    buf[bytesReceived] = '\0';
    message[fd] += string(buf);
    if (message[fd].find("\r\n") == std::string::npos)
        return {{"", "PARTIAL", ""}};
    
    istringstream stream(message[fd]);
    std::string line;
    std::vector<cmd> commands;
    
    while (getline(stream, line))
    {
        cmd cmd = {"", "", ""};
        istringstream lstream(line);
        if (line[0] == ':')
            getline(lstream, cmd.prefix, ' ');
        getline(lstream, cmd.command, ' ');
        getline(lstream, cmd.arguments, '\r');

        log(DEBUG, "RECV " + to_string(fd), line);
        
        commands.push_back(cmd);
    }
    message[fd] = "";
    return commands;
}