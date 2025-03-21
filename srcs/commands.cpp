#include "Server.hpp"

int countWords(const 	string &s) {
	size_t pos = s.find(":");
		string beforeColon = (pos != 	string::npos) ? s.substr(0, pos) : s;
	
		stringstream ss(beforeColon);
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

int	Server::PASS(cmd cmd, User &user) {
	if (cmd.arguments.empty()) {
		return (ERR_NEEDMOREPARAMS);
	} else if (cmd.arguments != this->_password) {
		return (ERR_PASSWDMISMATCH);
	} else if (user.getAuth()) {
		return (ERR_ALREADYREGISTRED);
	} else {
		user.setAuth(true);
		return (0);
	}
}

bool	Server::_nickIsUsed(string nick) {
	for (auto &[fd, user] : this->users) {
		if (user.getNickname() == nick) {
			return (true);
		}
	}
	return (false);
}

bool	Server::_userIsUsed(string username) {
	for (auto &[fd, user] : this->users) {
		if (user.getUsername() == username) {
			return (true);
		}
	}
	return (false);
}

int	Server::NICK(cmd cmd, User &user) {
	if (cmd.arguments.empty()) {
		return (ERR_NONICKNAMEGIVEN);
	} else if (_nickIsUsed(cmd.arguments)) {
		return (ERR_NICKNAMEINUSE);
	} else if (user.setNickname(cmd.arguments)) {
		return (ERR_ERRONEUSNICKNAME);
	}
	user.setNickIsSet(true);
	if (user.getUserIsSet() && !user.getIsRegistered()) {
		user.setIsRegistered(true);
		return (RPL_WELCOME);
	}
	return (0);
}

int	Server::USER(cmd cmd, User &user) {
	if (countWords(cmd.arguments) < 4) {
		return (ERR_NEEDMOREPARAMS);
	} else if (user.getUserIsSet()) {
		return (ERR_ALREADYREGISTRED); //only setup user to register
	}
	user.setUserIsSet(true);
	if (user.getNickIsSet() && !user.getIsRegistered()) {
		user.setIsRegistered(true);
		return (RPL_WELCOME);
	}
	return (0);
}

// int	Server::JOIN(cmd cmd, User &user) {
// 	string			res;
// 	log_level		type;
// 	stringstream	args(cmd.arguments);
// 	string			username;

// 	if (!user.getAuth()) {
// 		res = "Please login to set your nickname";
// 		type = ERROR;
// 	} else if (cmd.arguments.empty()) {
// 		res = "Empty arguments";
// 		type = ERROR;
// 	} else if (countWords(cmd.arguments) != 4) {
// 		res = "Invalid number of arguments";
// 		type = ERROR;
// 	}
// 	args >> username;
// 	if (user.getUsername() == username) {
// 		res = "You are already using this username";
// 		type = ERROR;
// 	} else if (_userIsUsed(username)) {
// 		res = "The username is already in use";
// 		type = ERROR;
// 	} else {
// 		if (user.setInfo(cmd.arguments)) {
// 			res = "Invalid argument formats";
// 			type = ERROR;			
// 		} else {
// 			res = "Your username has been set up";
// 			type = INFO;
// 		}
// 	}
// 	log(type, cmd.command, res);
// 	return (res);
// }

// int	Server::PRIVMSG(cmd cmd, User &user) {

// }

// int	Server::OPER(cmd cmd, User &user) {

// }


// int	Server::QUIT(cmd cmd, User &user) {

// }


// channel commands:

// int	Server::KICK(cmd cmd, User &user) { //Teemu

// }

// int	Server::INVITEcmd cmd, User &user) { //Teemu

// }

// int	Server::TOPIC(cmd cmd, User &user) { //Teemu

// }

// int	Server::MODE(cmd cmd, User &user) { //Teemu

// }
