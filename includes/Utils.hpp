#pragma once

#include "Server.hpp"
#define DEBUG_MODE true

enum log_level { DEBUG, INFO, WARN, ERROR };

#define RESET	"\033[0m";
#define RED		"\033[31m";
#define ORANGE	"\033[38;5;214m";
#define GREEN	"\033[32m";
#define BLUE	"\033[34m"

struct parsedArgs {
	vector <string>	args;
	string			trailing;
	int				size;
};

int 			countWords(const 	string &s);
vector<string>	commaSplit(string str);
bool			isValidChannelName(const string& channelName);
bool			matchesWildcard(const string &pattern, const string &target);
bool			targetIsUser(char c);
bool			isJoinedChannel(User &user, Channel &channel);
void 			log(log_level level, const string &event, const string &details);
parsedArgs		parseArgs(const std::string& args, int words, bool withTrailing);
string			trim(const string &str);
std::string 	toLowerString(std::string s);
