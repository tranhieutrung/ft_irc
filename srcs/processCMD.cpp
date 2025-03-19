#include "Server.hpp"

string	Server::_processPASS(cmd cmd, User &user) {
	string		res;
	log_level	type;

	if (cmd.arguments.empty()) { //will change to array of string so need to change condition to check empty password/ invlaid args
		res = "Empty password";
		type = ERROR;
	} else if (cmd.arguments != this->_password) {
		res = "Incorrect password";
		type = ERROR;
	} else {
		user.setAuth(true);
		res = "You logged in";
		type = INFO;
	}
	log(type, cmd.command, res);
	return (res);
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

string	Server::_processNICK(cmd cmd, User &user) {
	string		res;
	log_level	type;

	if (!user.getAuth()) {
		res = "Please login to set your nickname";
		type = ERROR;
	} else if (cmd.arguments.empty()) {
		res = "Empty nickname";
		type = ERROR;
	} else if (user.getNickname() == cmd.arguments) {
		res = "You are already using this nickname";
		type = ERROR;
	} else if (_nickIsUsed(cmd.arguments)) {
		res = "The nickname is already in use";
		type = ERROR;
	} else {
		if (user.setNickname(cmd.arguments)) {
			res = "Invalid nickname";
			type = ERROR;			
		} else {
			res = "Your nickname has been set up";
			type = INFO;
		}
	}
	log(type, cmd.command, res);
	return (res);
}

string	Server::_processUSER(cmd cmd, User &user) {
	string		res;
	log_level	type;

	if (!user.getAuth()) {
		res = "Please login to set your nickname";
		type = ERROR;
	} else if (cmd.arguments.empty()) {
		res = "Empty username";
		type = ERROR;
	} else if (user.getUsername() == cmd.arguments) {
		res = "You are already using this username";
		type = ERROR;
	} else if (_userIsUsed(cmd.arguments)) {
		res = "The username is already in use";
		type = ERROR;
	} else {
		if (user.setUsername(cmd.arguments)) {
			res = "Invalid username";
			type = ERROR;			
		} else {
			res = "Your username has been set up";
			type = INFO;
		}
	}
	log(type, cmd.command, res);
	return (res);
}

// string	Server::_processJOIN(cmd cmd, User &user) {

// }

// string	Server::_processPRIVMSG(cmd cmd, User &user) {

// }

// string	Server::_processOPER(cmd cmd, User &user) {

// }


// string	Server::_processQUIT(cmd cmd, User &user) {

// }


//channel commands:

// string	Server::_processKICK(cmd cmd, User &user) { //Teemu

// }

// string	Server::_processINVITEcmd cmd, User &user) { //Teemu

// }

// string	Server::_processTOPIC(cmd cmd, User &user) { //Teemu

// }

// string	Server::_processMODE(cmd cmd, User &user) { //Teemu

// }
