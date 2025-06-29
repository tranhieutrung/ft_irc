#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <poll.h>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <exception>
#include <optional>
#include <set>

// Forward declaration of User
class User;

using namespace std;

class Channel {
private:
    std::string ChannelName;
    std::string ChannelTopic;
    std::string password;
    std::map<int, User*> UserList;     
    std::map<int, const User*> InviteList;   
    std::set<int> operators;           
    bool inviteOnly;
    bool topic_restriction;
    unsigned int userLimit;

public:
    Channel() : ChannelName(""), ChannelTopic(""), password(""), inviteOnly(false), topic_restriction(false), userLimit(999) {}
    Channel(const std::string& name) : ChannelName(name), inviteOnly(false), topic_restriction(false), userLimit(999) {}
    Channel(const std::string& name, const std::string& pw) : ChannelName(name), ChannelTopic(""), password(pw), inviteOnly(false), topic_restriction(false), userLimit(999) {}

    // Getters
    std::string getChannelName() const { return ChannelName; }
    std::string getChannelTopic() const { return ChannelTopic; }
    std::string getPassword() const { return password; }
    const std::map<int, User*>& getUserList() const { return UserList; }
    const std::map<int, const User*>& getInviteList() const { return InviteList; }
    const std::set<int>& getOperators() const { return operators; }
    bool isInviteOnly() const { return inviteOnly; }
    bool isTopicRestricted() const { return topic_restriction; }
    unsigned int getUserLimit() const { return userLimit; }

    // Setters
    void setChannelName(const std::string& name) { ChannelName = name; }
    void setChannelTopic(const std::string& topic) { ChannelTopic = topic; }
    void setPassword(const std::string& pass) { password = pass; }
    void setInviteOnly(bool status) { inviteOnly = status; }
    void setTopicRestriction(bool status) { topic_restriction = status; }
    void setUserLimit(unsigned int limit) { userLimit = limit; }

    // User management
    void addUser(int fd, User* user);
    void removeUser(int fd);
    std::optional<std::map<int, User*>::iterator> findUser(int fd);
    std::optional<std::map<int, User*>::const_iterator> findUser(int fd) const;
    std::optional<std::map<int, User*>::iterator> findUserByNickname(const std::string& nickname);
    void addInvite(int fd, const User* user);
    void removeInvite(int fd);
    std::optional<std::map<int, const User*>::iterator> IsInvited(int fd);
    std::optional<std::map<int, const User*>::const_iterator> IsInvited(int fd) const;
    void addOperator(const User& user);
    void removeOperator(const User& user);
    bool isOperator(const User& user) const;
};
#endif
