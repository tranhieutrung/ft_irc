#include "Server.hpp"

string	Server::_processPASS(cmd cmd, User &user) {
	string		res;
	log_level	type;

	if (cmd.arguments.empty()) {
		res = "Invalid number of arguments";
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

// string	Server::_processNICK(cmd cmd, User &user) {

// }

// string	Server::_processUSER(cmd cmd, User &user) {

// }

// string	Server::_processOPER(cmd cmd, User &user) {

// }

// string	Server::_processMODE(cmd cmd, User &user) {

// }

// string	Server::_processPRIVMSG(cmd cmd, User &user) {

// }

// string	Server::_processJOIN(cmd cmd, User &user) {

// }

// string	Server::_processTOPIC(cmd cmd, User &user) {

// }

// string	Server::_processKICK(cmd cmd, User &user) {

// }

// string	Server::_processQUIT(cmd cmd, User &user) {

// }
