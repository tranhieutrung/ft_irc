#ifndef IRC_ERROR_CODES_HPP
#define IRC_ERROR_CODES_HPP

// --- Nickname and User Errors ---
#define ERR_NONICKNAMEGIVEN 431  
// "<client> :No nickname given"
#define ERR_ERRONEUSNICKNAME 432  
// "<client> <nick> :Erroneous nickname"
#define ERR_NICKNAMEINUSE 433  
// "<client> <nick> :Nickname is already in use"
#define ERR_NICKCOLLISION 436  
// "<client> <nick> :Nickname collision KILL"
#define ERR_UNAVAILRESOURCE 437  
// "<client> <nick/channel> :Nick/channel is temporarily unavailable"

// --- Registration & Authentication Errors ---
#define ERR_NOTREGISTERED 451  
// "<client> :You have not registered"
#define ERR_NEEDMOREPARAMS 461  
// "<client> <command> :Not enough parameters"
#define ERR_ALREADYREGISTRED 462  
// "<client> :You may not reregister"
#define ERR_PASSWDMISMATCH 464  
// "<client> :Password incorrect"
#define ERR_YOUREBANNEDCREEP 465  
// "<client> :You are banned from this server"
#define ERR_YOUWILLBEBANNED 466  
// "<client> :You will be banned"

// --- Channel Errors ---
#define ERR_CHANNELISFULL 471  
// "<client> <channel> :Cannot join channel (+l)"
#define ERR_UNKNOWNMODE 472  
// "<client> <char> :Unknown mode"
#define ERR_INVITEONLYCHAN 473  
// "<client> <channel> :Cannot join channel (+i)"
#define ERR_BANNEDFROMCHAN 474  
// "<client> <channel> :Cannot join channel (+b)"
#define ERR_BADCHANNELKEY 475  
// "<client> <channel> :Cannot join channel (+k)"
#define ERR_BADCHANMASK 476  
// "<client> <channel> :Bad Channel Mask"
#define ERR_NOCHANMODES 477  
// "<client> <channel> :Channel doesn't support modes"
#define ERR_BANLISTFULL 478  
// "<client> <channel> <char> :Ban list is full"

// --- Operator & Privilege Errors ---
#define ERR_NOPRIVILEGES 481  
// "<client> :Permission Denied- You're not an IRC operator"
#define ERR_CHANOPRIVSNEEDED 482  
// "<client> <channel> :You're not channel operator"
#define ERR_CANTKILLSERVER 483  
// "<client> :You can't kill a server!"
#define ERR_RESTRICTED 484  
// "<client> :Your connection is restricted"
#define ERR_UNIQOPPRIVSNEEDED 485  
// "<client> :You're not the original channel operator"

// --- Messaging & Command Errors ---
#define ERR_NOOPERHOST 491  
// "<client> :No O-lines for your host"
#define ERR_UMODEUNKNOWNFLAG 501  
// "<client> :Unknown MODE flag"
#define ERR_USERSDONTMATCH 502  
// "<client> :Cannot change mode for other users"

// --- Target Errors ---
#define ERR_NOSUCHNICK 401  
// "<client> <nick> :No such nick/channel"
#define ERR_NOSUCHSERVER 402  
// "<client> <server> :No such server"
#define ERR_NOSUCHCHANNEL 403  
// "<client> <channel> :No such channel"
#define ERR_CANNOTSENDTOCHAN 404  
// "<client> <channel> :Cannot send to channel"
#define ERR_TOOMANYCHANNELS 405  
// "<client> <channel> :You have joined too many channels"
#define ERR_WASNOSUCHNICK 406  
// "<client> <nick> :There was no such nickname"
#define ERR_TOOMANYTARGETS 407  
// "<client> <target> :Duplicate recipients. No message delivered"
#define ERR_NORECIPIENT 411  
// "<client> :No recipient given (<command>)"
#define ERR_NOTEXTTOSEND 412  
// "<client> :No text to send"
#define ERR_NOTOPLEVEL 413  
// "<client> <mask> :No toplevel domain specified"
#define ERR_WILDTOPLEVEL 414  
// "<client> <mask> :Wildcard in toplevel domain"

// --- Other Errors ---
#define ERR_UNKNOWNCOMMAND 421  
// "<client> <command> :Unknown command"
#define ERR_NOMOTD 422  
// "<client> :MOTD File is missing"
#define ERR_NOADMININFO 423  
// "<client> <server> :No administrative info available"
#define ERR_FILEERROR 424  
// "<client> :File error doing <file op> on <file>"

#endif // IRC_ERROR_CODES_HPP
