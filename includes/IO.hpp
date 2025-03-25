#ifndef IO_HPP
#define IO_HPP

#include <string>
#include <vector>

struct cmd
{
	std::string prefix;
	std::string command;
	std::string arguments;
};

class IO
{
	public:
		static ssize_t sendCommand(int fd, cmd cmd);
		static std::vector<cmd> recvCommands(int fd);
		IO() = delete;
};

#endif