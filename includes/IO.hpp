#ifndef IO_HPP
#define IO_HPP

#include <sstream>
#include <cstring>

using namespace std;

typedef struct
{
	string prefix;
	string command;
	string arguments;
} cmd;

class IO
{
	public:
		static ssize_t sendCommand(int fd, cmd cmd);
		static cmd recvCommand(int fd);
		IO() = delete;
};

#endif