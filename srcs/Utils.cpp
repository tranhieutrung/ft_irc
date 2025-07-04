#include "Utils.hpp"

int countWords(const 	string &s) {
	size_t pos = s.find(":");
	string beforeColon = (pos != 	string::npos) ? s.substr(0, pos) : s;
	
	istringstream ss(beforeColon);
	string word;
	int count = 0;

	while (ss >> word) {
		count++;
	}

	if (pos != 	string::npos) {
		count++;
	}

	return count;
}

vector<string> commaSplit(string str) {
	vector<string> result;

	if (str.empty()) {
		return result;
	}

	istringstream ss(str);
	string word;
	while (getline(ss, word, ',')) {
		result.push_back(word);
	}
	return (result);
}

/*
* Channels names are strings (beginning with a '&', '#', '+' or '!'
 character) of length up to fifty (50) characters. Apart from the
 requirement that the first character is either '&', '#', '+' or '!',
 the only restriction on a channel name is that it SHALL NOT contain
 any spaces (' '), a control G (^G or ASCII 7), a comma (','). Space
 is used as parameter separator and command is used as a list item
 separator by the protocol). A colon (':') can also be used as a
 delimiter for the channel mask. Channel names are case insensitive.
*/
bool isValidChannelName(const string& channelName) {
	if (channelName.empty() || channelName.size() > 50)
		return (false);
	regex channelRegex(R"(^[&#+!][^\x07\s,]*$)");
	return regex_match(channelName, channelRegex);
}


bool matchesWildcard(const string &pattern, const string &target) {
	string regexPattern = "^" + regex_replace(pattern, regex("\\*"), ".*") + "$";
	regex re(regexPattern);
	return regex_match(target, re);
}

bool targetIsUser(char c) {
	if (c == '#' || c == '&' || c == '+' || c == '!')
		return (false);
	return (true);
}

bool isJoinedChannel(User &user, Channel &channel) {
	for (const auto &it : channel.getUserList()) {
		if (it.first == user.getFd()) {
			return (true);
		}
	}
	return (false);
}

// trims spaces and : from end
string trim(const string &str)
{
	const string spaces = " \r\n\t\f\v:";
	size_t end;

	if (str.find_first_not_of(spaces) == string::npos)
		return ""; // if contains only spaces

	for (end = str.size() - 1;; --end)
		if (spaces.find(str[end]) == string::npos) // non-space char
			break;

	return str.substr(0, end + 1);
}

void log(const log_level level, const string &event, const string &details)
{
	if (details.find("PING") != string::npos || details.find("PONG") != string::npos)
		return; // no more flood in terminal

	if (level == DEBUG && DEBUG_MODE == false)
		return;

	time_t now = time(nullptr);
	tm *ltm = localtime(&now);

	cout << "[" << put_time(ltm, "%d.%m.%Y %H:%M:%S") << "] ";
	switch (level)
	{
		case INFO:
			cout << GREEN;
			cout << "[INFO] ";
			break;
		case WARN:
			cout << ORANGE;
			cout << "[WARN] ";
			break;
		case ERROR:
			cout << RED;
			cout << "[ERROR]";
			break;
		case DEBUG:
			cout << BLUE;
			cout << "[DEBUG]";
			break;
	}
	cout << RESET;
	cout << "[" << event << "] " << trim(details) << endl;
}

/*
Formats:
USER username hostname * [:]realname
PRIVMSG target [:]message
JOIN target1,target2... pw1,pw2...
QUIT [:][message]
PART target1,target2 [:][message]
*/
parsedArgs		parseArgs(const std::string& args, int argNum, bool withTrailing) {
	parsedArgs			result;
	std::istringstream	iss(args);
	std::string			temp;
	int 				limit = withTrailing ? (argNum - 1) : argNum;

	result.size = 0;
	while (result.size < limit && iss >> temp) {
		result.args.push_back(temp);
		result.size++;
	}

	if (withTrailing) {
		size_t 	pos = args.find(":");
		if (pos != std::string::npos) {
			result.trailing = args.substr(pos + 1);
		} else {
			iss >> result.trailing;
		}
		if (!result.trailing.empty()) {
			//cout << result.trailing <<endl;
			result.size++;
		}
	}
	return (result);
}


std::string toLowerString(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return result;
}

bool compareIgnoreCase(const std::string& a, const std::string& b) {
    return toLowerString(a) == toLowerString(b);
}