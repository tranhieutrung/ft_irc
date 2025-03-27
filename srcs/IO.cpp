#include "../includes/IO.hpp"
#include "../includes/Server.hpp"
#include <sstream>
#include <sys/socket.h>

ssize_t IO::sendCommand(int fd, cmd cmd)
{
    stringstream stream;
    stream << cmd.prefix << " ";
    stream << cmd.command << " ";
    stream << cmd.arguments << "\r\n";
    std::string sbuf = stream.str();

    log(DEBUG, "SEND", sbuf);

    return send(fd, sbuf.c_str(), sbuf.size(), 0);
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