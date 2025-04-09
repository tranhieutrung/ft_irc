#include "../includes/Server.hpp"

volatile sig_atomic_t Server::running = 1;

void Server::execute_command(cmd cmd, User &user)
{
	int code = 0;

	if (cmd.command != "QUIT" && cmd.command != "PASS" && user.getAuth() == false)
		return; // if not authenticated, ignore silently

	if (cmd.command == "PING") {
		code = PING(cmd, user);
	} else if (cmd.command == "PASS") {
		code = PASS(cmd, user); 
	} else if (cmd.command == "NICK") {
		code = NICK(cmd, user);
	} else if (cmd.command == "USER") {
		code = USER(cmd, user);
	} else if (cmd.command == "MODE") {
		code = MODE(cmd, user); 
	} else if (cmd.command == "QUIT") {
		code = QUIT(cmd, user); 
	// } else if (!user.getIsRegistered()) {
	// 	code = ERR_NOTREGISTERED; 
	// } else if (cmd.command == "OPER") {
	// 	code = OPER(cmd, user);
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
	} else if (cmd.command == "PART") {
		code = PART(cmd, user);
	} else if (cmd.command == "WHOIS") {
		code = WHOIS(cmd, user);
	} else if (cmd.command == "CAP") {
		return; // ignore CAP
	} else {
		code = ERR_UNKNOWNCOMMAND;
	}
	if (code) {
		sendMessage(code, cmd, user);
	}
	log(INFO, "COMMAND", user.getNickname() + " executed command " + cmd.command + " with code " + to_string(code));
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

		if (clientSocket == -1) {
			log(ERROR, "Connection", "Error accepting connection " + client_info(client_addr));
			cerr << "Error accepting connection" << endl;
			return;
		}
		pollfd new_pfd = {clientSocket, POLLIN, 0};
		fds.push_back(new_pfd);
		users[new_pfd.fd] = User(new_pfd.fd);

		log(INFO, "Connection", "New client connected: " + client_info(client_addr));
	}
}

void Server::handleClientMessages(size_t *index) {

	if ((fds[*index].revents & POLLIN) == false)
		return;

	int fd = fds[*index].fd;
	vector<cmd> commands = IO::recvCommands(fd);

	if (commands[0].command == "PARTIAL")
		return;

	if (commands[0].command != "DISCONNECT" && commands[0].command != "ERROR") {
		for (const auto &c : commands) {
			execute_command(c, users[fd]);
		}
		return;
	}

	if (commands[0].command == "DISCONNECT") {
		log(INFO, "Connection", "Client disconnected: " + users[fd].getNickname());
	} else {// "ERROR"
		close(fd);
		throw runtime_error("recv() failed");
	}

	execute_command({"", "QUIT", "disconnected"}, users[fd]);
	*index -= 1;
}

void Server::start() {

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	while (this->running)
	{
		if (poll(fds.data(), fds.size(), -1) < 0 && errno != EINTR)
			throw runtime_error("Poll error");

		handleNewClient();

		for (size_t index = 1; index < this->fds.size(); index++)
			handleClientMessages(&index);
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

Server::Server(const string port, const string password): _port(stoi(port)), _password(password) {
	fds.push_back({createSocket(), POLLIN, 0});
}

void Server::cleanup() {
	for (pollfd pfd : fds) {
		close(pfd.fd); // also close socket (fds[0]) here
	}
}

Server::~Server() {
	cleanup();
	log(INFO, "Server", "Shutting down server");
}

void Server::signal_handler(int signal) {
	if (signal == SIGINT || signal == SIGTERM)
		running = 0;
}

const User* Server::getUser(const string &nickname) {
	for (const auto &pair : users) {
		if (pair.second.getNickname() == nickname)
			return &pair.second;
	}
	return nullptr;
}

const User* Server::getUser(int fd) {
	if (users.find(fd) != users.end())
		return &users[fd];
	return nullptr;
}

Channel* Server::findChannelByName(const string& channelName) {
	auto it = this->channels.find(channelName);
	if (it != this->channels.end()) {
		return &it->second;
	}
	return nullptr;
}

User* Server::findUserByNickName(const string& nickName) {
	for (auto &it : this->users) {
		if (it.second.getNickname() == nickName)
			return (&it.second);
	}
	return nullptr;
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
	for (auto &it : this->users) {
		if (it.second.getUsername() == username) {
			return (true);
		}
	}
	return (false);
}

//user create and join a new channel
int Server::createChannel(Channel*& channel, User &user, const std::string &channelName, const std::string &key) {
	auto [it, inserted] = this->channels.emplace(channelName, Channel(channelName, key));
	channel = &it->second;

	int code = user.join(*channel, key);
	if (!code) {
		channel->addOperator(user);
	}
	return code;
}

void Server::removeUser(int UserFd) {
	shutdown(UserFd, SHUT_RDWR);
	close(UserFd);
	this->users.erase(UserFd);
	this->fds.erase(
		std::remove_if(this->fds.begin(), this->fds.end(),
			[UserFd](const struct pollfd &pfd) {
				return pfd.fd == UserFd;
			}),
			this->fds.end()
	);
	log(INFO, "Connection", "Client disconnected: fd " + std::to_string(UserFd));
}
