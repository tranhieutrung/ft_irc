#ifndef IO_HPP
#define IO_HPP

// #include "User.hpp"
#include <string>
#include <vector>
#include <map>

struct cmd
{
	std::string prefix;
	std::string command;
	std::string arguments;
};

class User;

class IO
{
	public:
		IO() = delete;
		static std::vector<cmd> recvCommands(int fd);
		static ssize_t sendCommand(int fd, const cmd &cmd);
		static ssize_t sendString(int fd, const std::string &s);
		static ssize_t sendCommandAll(const std::map<int, User> m, const cmd &cmd);
		static ssize_t sendCommandAll(const std::map<std::string, User> m, const cmd &cmd);
		static ssize_t sendStringAll(const std::map<int, User> m, const std::string &s);
		static ssize_t sendStringAll(const std::map<std::string, User> m, const std::string &s);
};

#endif