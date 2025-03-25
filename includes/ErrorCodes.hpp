#ifndef IRC_ERROR_CODES_HPP
#define IRC_ERROR_CODES_HPP



// --- Nickname and User Errors ---
#define ERR_NONICKNAMEGIVEN 431  
// "<client> :No nickname given"
// Returned when a nickname parameter is expected but not provided.

#define ERR_ERRONEUSNICKNAME 432  
// "<client> <nick> :Erroneous nickname"
// Returned when the given nickname is invalid (e.g., contains forbidden characters).

#define ERR_NICKNAMEINUSE 433  
// "<client> <nick> :Nickname is already in use"
// Returned when the chosen nickname is already taken by another user.

#define ERR_NICKCOLLISION 436  
// "<client> <nick> :Nickname collision KILL"
// Returned when two servers detect a user with the same nickname, causing a forced disconnection.

#define ERR_UNAVAILRESOURCE 437  
// "<client> <nick/channel> :Nick/channel is temporarily unavailable"
// Indicates a nickname or channel is temporarily unavailable due to a conflict or restriction.

// --- Registration & Authentication Errors ---
#define ERR_NOTREGISTERED 451  
// "<client> :You have not registered"
// Returned when the client tries to execute a command before completing registration.

#define ERR_NEEDMOREPARAMS 461  
// "<client> <command> :Not enough parameters"
// Returned when a command lacks the required number of parameters.

#define ERR_ALREADYREGISTRED 462  
// "<client> :You may not reregister"
// Returned when a user attempts to register again after already being registered.

#define ERR_PASSWDMISMATCH 464  
// "<client> :Password incorrect"
// Returned when an incorrect server password is provided during authentication.

#define ERR_YOUREBANNEDCREEP 465  
// "<client> :You are banned from this server"
// Indicates the user is banned from the server and cannot connect.

#define ERR_YOUWILLBEBANNED 466  
// "<client> :You will be banned"
// Sent before forcibly disconnecting a user as a warning of an imminent ban.

// --- Channel Errors ---
#define ERR_CHANNELISFULL 471  
// "<client> <channel> :Cannot join channel (+l)"
// Returned when attempting to join a channel that has reached its user limit.

#define ERR_UNKNOWNMODE 472  
// "<client> <char> :Unknown mode"
// Returned when an unknown mode character is used in a mode command.

#define ERR_INVITEONLYCHAN 473  
// "<client> <channel> :Cannot join channel (+i)"
// Returned when attempting to join an invite-only channel without an invitation.

#define ERR_BANNEDFROMCHAN 474  
// "<client> <channel> :Cannot join channel (+b)"
// Returned when a banned user attempts to join a channel.

#define ERR_BADCHANNELKEY 475  
// "<client> <channel> :Cannot join channel (+k)"
// Returned when attempting to join a password-protected channel without the correct password.

#define ERR_BADCHANMASK 476  
// "<client> <channel> :Bad Channel Mask"
// Returned when an invalid channel name is used.

#define ERR_NOCHANMODES 477  
// "<client> <channel> :Channel doesn't support modes"
// Returned when trying to set modes on a channel that does not support them.

#define ERR_BANLISTFULL 478  
// "<client> <channel> <char> :Ban list is full"
// Returned when trying to add a user to a full ban list.

// --- Operator & Privilege Errors ---
#define ERR_NOPRIVILEGES 481  
// "<client> :Permission Denied- You're not an IRC operator"
// Returned when a user tries to perform an operator-only action without privileges.

#define ERR_CHANOPRIVSNEEDED 482  
// "<client> <channel> :You're not channel operator"
// Returned when a non-operator tries to perform a channel operator action.

#define ERR_CANTKILLSERVER 483  
// "<client> :You can't kill a server!"
// Returned when a user attempts the KILL command on a server.

#define ERR_RESTRICTED 484  
// "<client> :Your connection is restricted"
// Returned when a user with a restricted connection attempts an action they're not allowed to perform.

#define ERR_UNIQOPPRIVSNEEDED 485  
// "<client> :You're not the original channel operator"
// Returned when an action requires the unique operator privilege.

// --- Messaging & Command Errors ---
#define ERR_NOOPERHOST 491  
// "<client> :No O-lines for your host"
// Returned when an OPER command is attempted from an unauthorized host.

#define ERR_UMODEUNKNOWNFLAG 501  
// "<client> :Unknown MODE flag"
// Returned when attempting to set an unknown user mode.

#define ERR_USERSDONTMATCH 502  
// "<client> :Cannot change mode for other users"
// Returned when a user attempts to modify another user's mode without proper permissions.

// --- Target Errors ---
#define ERR_NOSUCHNICK 401  
// "<client> <nick> :No such nick/channel"
// Returned when a command is sent to a nonexistent nickname or channel.

#define ERR_NOSUCHSERVER 402  
// "<client> <server> :No such server"
// Returned when a server name in a command is invalid or unreachable.

#define ERR_NOSUCHCHANNEL 403  
// "<client> <channel> :No such channel"
// Returned when referencing a channel that does not exist.

#define ERR_CANNOTSENDTOCHAN 404  
// "<client> <channel> :Cannot send to channel"
// Returned when a user cannot send a message to a channel (e.g., due to +m or +b mode).

#define ERR_TOOMANYCHANNELS 405  
// "<client> <channel> :You have joined too many channels"
// Returned when a user has reached the maximum number of joined channels.

#define ERR_WASNOSUCHNICK 406  
// "<client> <nick> :There was no such nickname"
// Returned when querying a nickname that was recently but is no longer in use.

#define ERR_TOOMANYTARGETS 407  
// "<client> <target> :Duplicate recipients. No message delivered"
// Returned when a message or command targets too many users or channels.

#define ERR_NORECIPIENT 411  
// "<client> :No recipient given (<command>)"
// Returned when a command requiring a recipient (e.g., PRIVMSG) is missing one.

#define ERR_NOTEXTTOSEND 412  
// "<client> :No text to send"
// Returned when a command requiring a message text (e.g., PRIVMSG) has no text.

#define ERR_NOTOPLEVEL 413  
// "<client> <mask> :No toplevel domain specified"
// Returned when a server mask is missing a top-level domain.

#define ERR_WILDTOPLEVEL 414  
// "<client> <mask> :Wildcard in toplevel domain"
// Returned when a wildcard is used improperly in a top-level domain.

// --- Other Errors ---
#define ERR_UNKNOWNCOMMAND 421  
// "<client> <command> :Unknown command"
// Returned when an unrecognized command is received.

#define ERR_NOMOTD 422  
// "<client> :MOTD File is missing"
// Returned when the server's Message of the Day (MOTD) is unavailable.

#define ERR_NOADMININFO 423  
// "<client> <server> :No administrative info available"
// Returned when administrative details for a server are unavailable.

#define ERR_FILEERROR 424  
// "<client> :File error doing <file op> on <file>"
// Returned when a server encounters a file-related error.

#define ERR_NOLOGIN 444
//  "<user> :User not logged in"

#define ERR_NOORIGIN 409
//":No origin specified"
// PING or PONG message missing the originator parameter.

#define ERR_NOSUCHSERVER 402
// "<server name> :No such server"
// Used to indicate the server name given currently does not exist.

#define ERR_NOTONCHANNEL	442
// "<channel> :You're not on that channel"
// Returned by the server whenever a client tries to perform a channel affecting 
// command for which the client isn't a member.

#endif // IRC_ERROR_CODES_HPP
