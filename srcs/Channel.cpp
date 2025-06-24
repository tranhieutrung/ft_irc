/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttero <ttero@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 11:47:53 by ttero             #+#    #+#             */
/*   Updated: 2025/06/23 11:47:15 by ttero            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Channel.hpp"
#include <optional>
#include "User.hpp"


std::optional<std::map<int, User>::iterator> Channel::findUserByNickname(const std::string& nickname) {
    for (auto it = UserList.begin(); it != UserList.end(); ++it) {
        if (compareIgnoreCase(it->second.getNickname(),nickname)) {
            return it;
        }
    }
    return std::nullopt;
}

std::optional<std::map<int, User>::iterator> Channel::findUser(int fd) {
    auto it = UserList.find(fd);
    if (it != UserList.end()) {
        return it;
    }
    return std::nullopt;
}

std::optional<std::map<int, User>::const_iterator> Channel::findUser(int fd) const {
    auto it = UserList.find(fd);
    if (it != UserList.end()) {
        return it;
    }
    return std::nullopt;
}

void Channel::addUser(int fd, const User& user) {
    UserList[fd] = user;
}

void Channel::removeUser(int fd) {
    UserList.erase(fd);
	for (auto pair : UserList)
		log(DEBUG, "Channel::removeUser", std::to_string(pair.first));
}

void Channel::addOperator(const User& user) {
    if (!isOperator(user)) {
        operators.push_back(user);
    }
}
void Channel::removeOperator(const User& user){
    auto it = std::find(operators.begin(), operators.end(), user);
    if (it != operators.end()) {
        operators.erase(it);
    }
}
bool Channel::isOperator(const User& user) const {
    return std::find(operators.begin(), operators.end(), user) != operators.end();
}

std::optional<std::map<int, User>::iterator> Channel::IsInvited(int fd) {
    auto it = InviteList.find(fd);
    if (it != InviteList.end()) {
        return it;
    }
    return std::nullopt;
}

std::optional<std::map<int, User>::const_iterator> Channel::IsInvited(int fd) const {
    auto it = InviteList.find(fd);
    if (it != InviteList.end()) {
        return it;
    }
    return std::nullopt;
}

void Channel::addInvate(int fd, const User& user) {
    InviteList[fd] = user;
}

void Channel::removeInvate(int fd) {
    InviteList.erase(fd);
}
