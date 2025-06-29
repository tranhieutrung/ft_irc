/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttero <ttero@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 11:47:53 by ttero             #+#    #+#             */
/*   Updated: 2025/06/29 13:49:07 by ttero            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Channel.hpp"
#include <optional>
#include "User.hpp"


void Channel::addUser(int fd, User* user) {
    UserList[fd] = user;
}

void Channel::removeUser(int fd) {
    UserList.erase(fd);
}

std::optional<std::map<int, User*>::iterator> Channel::findUser(int fd) {
    auto it = UserList.find(fd);
    if (it != UserList.end())
        return it;
    return std::nullopt;
}

std::optional<std::map<int, User*>::const_iterator> Channel::findUser(int fd) const {
    auto it = UserList.find(fd);
    if (it != UserList.end())
        return it;
    return std::nullopt;
}

std::optional<std::map<int, User*>::iterator> Channel::findUserByNickname(const std::string& nickname) {
    for (auto it = UserList.begin(); it != UserList.end(); ++it) {
        if (compareIgnoreCase(it->second->getNickname(), nickname)) {
            return it;
        }
    }
    return std::nullopt;
}

void Channel::addInvite(int fd, const User* user) {
    InviteList[fd] = user;
}

void Channel::removeInvite(int fd) {
    InviteList.erase(fd);
}

std::optional<std::map<int, const User *>::iterator> Channel::IsInvited(int fd) {
    auto it = InviteList.find(fd);
    if (it != InviteList.end())
        return it;
    return std::nullopt;
}

std::optional<std::map<int, const User *>::const_iterator> Channel::IsInvited(int fd) const {
    auto it = InviteList.find(fd);
    if (it != InviteList.end())
        return it;
    return std::nullopt;
}
void Channel::addOperator(const User& user) {
    operators.insert(user.getFd());
}

void Channel::removeOperator(const User& user) {
    operators.erase(user.getFd());
}

bool Channel::isOperator(const User& user) const {
    return operators.find(user.getFd()) != operators.end();
}