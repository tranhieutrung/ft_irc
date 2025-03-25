/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttero <ttero@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 11:47:53 by ttero             #+#    #+#             */
/*   Updated: 2025/03/22 19:45:59 by ttero            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <optional>
#include "User.hpp"



std::optional<std::map<string, User>::iterator> Channel::findUser(string username)
 {
    auto it = UserList.find(username);
    if (it != UserList.end()) {
        return it;
    }
    return std::nullopt;
}

void Channel::addUser(const string& username, const User& user)
{
	UserList[username] = user;
}
void Channel::removeUser(string user)
{
	UserList.erase(user);
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
