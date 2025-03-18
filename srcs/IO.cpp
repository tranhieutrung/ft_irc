#include "IO.hpp"
#include <sstream>
#include <sys/socket.h>

ssize_t IO::sendCommand(int fd, cmd cmd)
{
    stringstream stream;
    stream << cmd.prefix << " ";
    stream << cmd.command << " ";
    stream << cmd.arguments << "\r\n";
    const char *buf = stream.str().c_str();
    return send(fd, buf, sizeof(buf), 0);
}

cmd IO::recvCommand(int fd)
{
    char buf[512];
    ssize_t bytesReceived = recv(fd, buf, sizeof(buf), 0);

    if (bytesReceived == 0)
        return {"", "DISCONNECT", ""};
    if (bytesReceived < 0)
        return {"", "ERROR", ""};
    cmd cmd;
    istringstream stream(buf);
    if (buf[0] == ':')  
        stream >> cmd.prefix;
    stream >> cmd.command;
    getline(stream, cmd.arguments, '\r');
    return cmd;
}