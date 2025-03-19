#ifndef IO_HPP
#define IO_HPP

#include <string>

using namespace std;

struct cmd
{
	string prefix;
	string command;
	string arguments;
};

class IO
{
	public:
		static ssize_t sendCommand(int fd, cmd cmd);
		static cmd recvCommand(int fd);
		IO() = delete;
};

#endif