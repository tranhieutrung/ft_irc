### ft_irc — Internet Relay Chat server (42 project)

ft_irc is a minimal yet compliant IRC server written in C++20. It supports basic RFC-like semantics for registration, channels, messaging, and channel moderation. You can connect using any IRC client (e.g., irssi, Weechat) or via raw TCP for testing.

This README explains how to build, run, connect, and use the server, and lists the implemented commands and modes.

## Features
- **Registration flow**: `PASS`, `NICK`, `USER`, welcome after both NICK and USER are set with a valid password
- **Presence**: `PING`/`PONG`, `QUIT`
- **Messaging**: `PRIVMSG` to users or channels
- **Channels**: `JOIN`, `PART`, `TOPIC`, `INVITE`, `KICK`
- **Modes (channel)**: `MODE` with flags `+i/-i` (invite-only), `+t/-t` (topic change restricted), `+k/-k` (key/password), `+l/-l` (user limit), `+o/-o` (op add/remove)
- **Whois**: `WHOIS <nick>` reports user info
- **Replies/Errors**: Uses numeric reply and error codes (see `includes/ReplyCodes.hpp`, `includes/ErrorCodes.hpp`)

## Project layout
- `includes/` — headers for `Server`, `User`, `Channel`, IO helpers and numeric codes
- `srcs/` — server implementation, command handlers, utilities
- `Makefile` — build rules (C++20)
- `start.sh` — example run under valgrind

## Build
Requirements (Linux):
- C++20 compiler (g++ 10+ or clang++)
- POSIX sockets (`netinet/in.h`, `arpa/inet.h`, `poll.h`, etc.)

Build the server binary `ircserv`:
```bash
make
```
Clean artifacts:
```bash
make clean    # remove object files
make fclean   # remove objects and binary
make re       # full rebuild
```

## Run
Usage:
```bash
./ircserv <port> <password>
```

Constraints validated at startup:
- **Port**: 6660–6669 or 6697
- **Password**: alphanumeric only, length 3–20

Example:
```bash
./ircserv 6667 pass123
```

For leak checking (example helper):
```bash
valgrind -q --leak-check=full ./ircserv 6667 pass
```

## Connecting with an IRC client
You can use any IRC client. Examples below use `irssi`:

1) Start server:
```bash
./ircserv 6667 pass123
```

2) In another terminal, launch irssi and connect:
```bash
irssi
/server add -password pass123 localhost 6667
/connect localhost 6667 pass123
```

3) Register (irssi usually sends these automatically):
```text
/nick alice
/set real_name "Alice"
/set user_name "alice"
/quote USER alice 0 * :Alice
```

4) Create/join a channel and chat:
```text
/join #chat
/msg #chat hello world
```

## Raw IRC testing (telnet/netcat)
You can also test with raw TCP. After connecting, send the registration sequence:
```text
PASS pass123
NICK alice
USER alice 0 * :Alice Doe
JOIN #chat
PRIVMSG #chat :hi from raw client
QUIT :bye
```

## Implemented commands
Registration and session:
- `PASS <password>` — must be valid and sent before registration completes
- `NICK <nickname>` — unique nickname; server replies with welcome when both NICK and USER are set
- `USER <username> <hostname> <servername> :<realname>` — minimal checks; username is uniqued if taken
- `QUIT [:message]` — leaves all channels and disconnects

Health and info:
- `PING <server>` → `PONG` (server name is `IRCS` internally)
- `PONG <full-identifier>` — no-op acknowledgement
- `WHOIS <nick>` — returns user info or error if not found

Messaging:
- `PRIVMSG <target> :<message>` — `<target>` is a nick or a channel

Channels:
- `JOIN <channel>[,<channel>...] [<key>[,<key>...]]` — supports multiple targets; `JOIN 0` parts all
- `PART <channel>[,<channel>...] [:message]`
- `TOPIC <channel> [:new-topic]` — view or set topic; setting may require op depending on mode
- `INVITE <nick> <channel>` — requires channel operator
- `KICK <channel> <nick>` — requires channel operator

Modes (channel):
- `MODE <channel>` — shows current modes
- `MODE <channel> +i | -i` — set/clear invite-only
- `MODE <channel> +t | -t` — restrict/unrestrict topic changes to ops
- `MODE <channel> +k <key> | -k` — set/clear channel key
- `MODE <channel> +l <n> | -l` — set/clear user limit
- `MODE <channel> +o <nick> | -o <nick>` — op/deop a user

## Channel semantics (high level)
- Channel names are validated, and a new channel is created on first `JOIN`
- First user to create a channel becomes an operator
- Invite-only channels require `INVITE` to join
- Topic changes can be restricted to operators via `+t`
- Channel key (`+k`) and user limit (`+l`) are enforced on join
- Empty channels are removed automatically when the last user leaves

## Error handling and numerics
The server returns common IRC numeric replies and errors, e.g.:
- Success/info: `001` (welcome), `311` (WHOIS user), `332` (topic), `353/366` (names/end)
- Errors: need more params, bad channel mask, no such nick/channel, nick in use, already registered, not on channel, channel operator privileges needed, etc.
See `includes/ReplyCodes.hpp` and `includes/ErrorCodes.hpp` for the exact codes used internally.

## Notes & limitations
- Designed and tested for Linux (POSIX sockets). Not supported on Windows without a POSIX layer.
- TLS/SSL (6697) port is validated but TLS is not implemented; use plaintext connections.
- Operator (`OPER`) command is not implemented; channel ops are managed via `MODE +o/-o`.

## License
This project is for educational purposes as part of 42’s curriculum. Check your campus guidelines before reusing.

