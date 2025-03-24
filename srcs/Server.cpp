#include "../includes/Server.hpp"

volatile sig_atomic_t Server::running = 1;

// Private members:
string	Server::commandResponses(int code, cmd cmd, User &user) {
	string message;

	message = ":" + this->_name + " ";
	if (code <= 5) {
		message += "00";
	}
	message += to_string(code) + " " + user.getNickname() + " ";
	
	if (code == RPL_WELCOME) {
		message += ":Welcome to the Internet Relay Network " + user.getFullIdentifier();
	} else if (code == ERR_NEEDMOREPARAMS) {
		message += cmd.command + " :Not enough parameters";
	} else if (code == ERR_PASSWDMISMATCH) {
		message += ":Password incorrect";
	} else if (code == ERR_ALREADYREGISTRED) {
		message += ":Unauthorized command (already registered)";
	} else if (code == ERR_NOLOGIN) {
		message += user.getUsername() + " :User not logged in";
	} else if (code == ERR_NONICKNAMEGIVEN) {
		message += ":No nickname given";
	} else if (code == ERR_NICKNAMEINUSE) {
		message += cmd.arguments + " :Nickname is already in use";
	} else if (code == ERR_ERRONEUSNICKNAME) {
		message += cmd.arguments + " :Erroneous nickname";
	} else if (code == ERR_UNKNOWNCOMMAND) {
		message += cmd.command + " :Unknown command";
	} else if (code == ERR_NOTREGISTERED) {
		message += ":You have not registered";
	} else if (code == ERR_NOORIGIN) {
		message += ":No origin specified";
	} else if (code == ERR_NOSUCHSERVER) {
		message += cmd.arguments + " :No such server";
	}
	message += "(hitran)\r\n";

	return (message);
}


void Server::execute_command(cmd cmd, User &user)
{
	int code;

	if (cmd.command == "PING") {
		code = PING(cmd, user);
	} else if (cmd.command == "PASS") {
		code = PASS(cmd, user); 
	//} else if (!user.getAuth()) {
	// 	code = ERR_NOLOGIN;
	} else if (cmd.command == "NICK") {
		code = NICK(cmd, user);
	} else if (cmd.command == "USER") {
		code = USER(cmd, user);
	} else if (cmd.command == "MODE") {
		code = MODE(cmd, user); 
	} else if (!user.getIsRegistered()) {
		code = ERR_NOTREGISTERED; 
	} else if (cmd.command == "OPER") {
		code = OPER(cmd, user);
	} else if (cmd.command == "INVITE") {
		code = INVITE(cmd, user); 
	} else if (cmd.command == "PRIVMSG") {
		code = PRIVMSG(cmd, user); 
	} else if (cmd.command == "JOIN") {
		code = JOIN(cmd, user); 
	} else if (cmd.command == "TOPIC") {
		code = TOPIC(cmd, user); 
	} else if (cmd.command == "KICK") {
		code = KICK(cmd, user); 
	} else if (cmd.command == "QUIT") {
		code = QUIT(cmd, user); 
	} else if (cmd.command == "PART") {
		code = PART(cmd, user);
	} else {
		code = ERR_UNKNOWNCOMMAND;
	}
	string message = commandResponses(code, cmd, user);
	if (code && send(user.getFd(), message.c_str(), message.length(), 0) == -1)
		cerr << "send() error: " << strerror(errno) << endl;
}

static cmd parse_line(string &message)
{
	cmd cmd;
	istringstream stream(message);
	if (message[0] == ':')
		stream >> cmd.prefix;
	stream >> cmd.command;
	stream.ignore(1); // skip space
	getline(stream, cmd.arguments, '\r');
	return cmd;
}

string Server::client_info(struct sockaddr_in &client_addr)
{
	return "IP: " + string(inet_ntoa(client_addr.sin_addr)) 
	+ " Port: " + to_string(ntohs(client_addr.sin_port));
}

void Server::handleNewClient()
{
	if (fds[0].revents & POLLIN) {
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		int clientSocket = accept(fds[0].fd, (struct sockaddr *)&client_addr, &client_len);

		if (clientSocket == -1)
		{
			log(ERROR, "Connection", "Error accepting connection " + client_info(client_addr));
			cerr << "Error accepting connection" << endl;
			return;
		}
		pollfd new_pfd = {clientSocket, POLLIN, 0};
		fds.push_back(new_pfd);
		users[new_pfd.fd] = User(new_pfd.fd);

		// sendMessageToClient(RPL_WELCOME, "", users[new_pfd.fd]);
		log(INFO, "Connection", "New client connected: " + client_info(client_addr));
	}
}

void Server::process_message(int clientFd, string buffer)
{
	stringstream message;
	message << buffer; 
	string line;
	while (getline(message, line)) {
		execute_command(parse_line(line), users[clientFd]);
	}
}

void Server::handleClientMessages(int i)
{
	if (fds[i].revents & POLLIN)
	{
		char buffer[1024] = {0}; // what if over 1024
		int bytesReceived = recv(fds[i].fd, buffer, sizeof(buffer), 0);
		
		if (bytesReceived > 0) {
			process_message(fds[i].fd, buffer);
		} else {
			if (bytesReceived == 0) {
				log(INFO, "Connection", "Client disconnected: " + users[fds[i].fd].getNickname()); //if client has set their Nick/Username, it will null
			} else {
				cerr << "recv() failed: " << strerror(errno) << endl;
			}
			close(fds[i].fd);
			users.erase(fds[i].fd);
			fds.erase(fds.begin() + i);
			i--;
		}
	}
}

void Server::cleanup()
{
	for (pollfd pfd : fds) {
		close(pfd.fd); // also close socket (fds[0]) here
	}
}

int Server::createSocket() {
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)  {
		throw runtime_error("Error: socket failed: " + string(strerror(errno)));
	}

	int opt = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		throw runtime_error("setsockopt failed: " + string(strerror(errno)));
	}
	
	sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(this->_port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
		close (serverSocket);
		throw runtime_error("binding failed: " + string(strerror(errno)));
	}

	if (listen(serverSocket, _maxClients) == -1) {
		close (serverSocket);
		throw runtime_error("listening failed: " + string(strerror(errno)));
	}

	log(INFO, "Server", "Server started on port " + to_string(_port));
	return serverSocket;
}

// Public members

Server::Server(const string port, const string password): _port(stoi(port)), _password(password) {
	fds.push_back({createSocket(), POLLIN, 0});
}

Server::~Server() {
	cleanup();
	log(INFO, "Server", "Shutting down server");
}

void Server::start()
{

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	while (this->running)
	{
		if (poll(fds.data(), fds.size(), -1) < 0 && errno != EINTR)
			throw runtime_error("Poll error");
		for (size_t index = 0; index < this->fds.size(); index++) {
			if (index == 0) { // fds[0] = serverSocket
				handleNewClient();
			} else {
				handleClientMessages(index);
			}
		}
	}
}

void Server::signal_handler(int signal)
{
	if (signal == SIGINT || signal == SIGTERM)
		running = 0;
}

const User* Server::getUser(const string &nickname)
{
	for (const auto &pair : users)
	{
		if (pair.second.getNickname() == nickname)
			return &pair.second;
	}
	return nullptr;
}

const User* Server::getUser(int fd)
{
	if (users.find(fd) != users.end())
		return &users[fd];
	return nullptr;
}

void Server::log(log_level level, const string &event, const string &details)
{
	time_t now = time(nullptr);
	tm *ltm = localtime(&now);

	const string RESET = "\033[0m";
	const string RED = "\033[31m";
	const string ORANGE = "\033[38;5;214m";
	const string GREEN = "\033[32m";

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
			cout << "[ERROR] "; //cerr?
			break;
	}
	cout << RESET;
	cout << "[" << event << "] " << details << endl;
}
