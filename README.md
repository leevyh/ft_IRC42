socket, close, setsockopt, getsockname,
getprotobyname, gethostbyname, getaddrinfo,
freeaddrinfo, bind, connect, listen, accept, htons,
htonl, ntohs, ntohl, inet_addr, inet_ntoa, send,
recv, signal, sigaction, lseek, fstat, fcntl, poll
(or equivalent)


Even if poll() is mentionned in the subject and the evaluation scale,
you can use any equivalent such as select(), kqueue(), or epoll().

Because you have to use non-blocking file descriptors, it is
possible to use read/recv or write/send functions with no poll()
(or equivalent), and your server wouldn’t be blocking.
But it would consume more system resources.
Thus, if you try to read/recv or write/send in any file descriptor
without using poll() (or equivalent), your grade will be 0.


You must be able to :
- authenticate, 
- set a nickname, 
- set a username, 
- join a channel,
- send and receive private messages using your reference client.

◦ All the messages sent from one client to a channel have to be forwarded to every other client that joined the channel.

◦ You must have operators and regular users.

◦ Then, you have to implement the commands that are specific to channel operators:
	∗ KICK - Eject a client from the channel
	∗ INVITE - Invite a client to a channel
	∗ TOPIC - Change or view the channel topic
	∗ MODE - Change the channel’s mode:
		· i: Set/remove Invite-only channel
		· t: Set/remove the restrictions of the TOPIC command to channel operators
		· k: Set/remove the channel key (password)
		· o: Give/take channel operator privilege
		· l: Set/remove the user limit to channel


Command responses

Numerics in the range from 001 to 099 are used for client-server connections only and should never travel between servers. Replies generated in the response to commands are found in the range from 200 to 399.

001	RPL_WELCOME
		"Welcome to the Internet Relay Network <nick>!<user>@<host>"
002	RPL_YOURHOST
		"Your host is <servername>, running version <ver>"
003	RPL_CREATED
		"This server was created <date>"
004	RPL_MYINFO
		"<servername> <version> <available user modes> <available channel modes>"
- The server sends Replies 001 to 004 to a user upon successful registration.




Command: WHO
Parameters: [ <mask> [ "o" ] ]

The WHO command is used by a client to generate a query which returns a list of information which 'matches' the <mask> parameter given by the client.  In the absence of the <mask> parameter, all visible (users who aren't invisible (user mode +i) and who don't have a common channel with the requesting client) are listed.  The same result can be achieved by using a <mask> of "0" or any wildcard which will end up matching every visible user.

The <mask> passed to WHO is matched against users' host, server, real name and nickname if the channel <mask> cannot be found.
If the "o" parameter is passed only operators are returned according to the <mask> supplied.

311	RPL_WHOISUSER
		"<nick> <user> <host> * :<real name>"
312	RPL_WHOISSERVER
		"<nick> <server> :<server info>"
318	RPL_ENDOFWHOIS
		"<nick> :End of WHOIS list"
319	RPL_WHOISCHANNELS
		"<nick> :*( ( "@" / "+" ) <channel> " " )"
- Replies 311 - 313, 317 - 319 are all replies generated in response to a WHOIS message.  Given that there are enough parameters present, the answering server MUST either formulate a reply out of the above numerics (if the query nick is found) or return an error reply.  The '*' in RPL_WHOISUSER is there as the literal character and not as a wild card.  For each reply set, only RPL_WHOISCHANNELS may appear more than once (for long lists of channel names).
The '@' and '+' characters next to the channel name indicate whether a client is a channel operator or has been granted permission to speak on a moderated channel.  The RPL_ENDOFWHOIS reply is used to mark the end of processing a WHOIS message.


433	ERR_NICKNAMEINUSE
		"<nick> :Nickname is already in use"


Command: PING
Parameters: <server1> [ <server2> ]

The PING command is used to test the presence of an active client or server at the other end of the connection.  Servers send a PING message at regular intervals if no other activity detected coming from a connection.  If a connection fails to respond to a PING message within a set amount of time, that connection is closed.  A PING message MAY be sent even if the connection is active.
When a PING message is received, the appropriate PONG message MUST be sent as reply to <server1> (server which sent the PING message out) as soon as possible.  If the <server2> parameter is specified, it represents the target of the ping, and the message gets forwarded there.

Command: PONG
Parameters: <server> [ <server2> ]

PONG message is a reply to ping message.  If parameter <server2> is given, this message MUST be forwarded to given target.  The <server> parameter is the name of the entity who has responded to PING message and generated this message.


Command: JOIN
Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"

The JOIN command is used by a user to request to start listening to the specific channel.  Servers MUST be able to parse arguments in the form of a list of target, but SHOULD NOT use lists when sending JOIN messages to clients.
Once a user has joined a channel, he receives information about all commands his server receives affecting the channel.  This includes JOIN, MODE, KICK, PART, QUIT and of course PRIVMSG/NOTICE. This allows channel members to keep track of the other channel members, as well as channel modes.
If a JOIN is successful, the user receives a JOIN message as confirmation and is then sent the channel's topic (using RPL_TOPIC) and the list of users who are on the channel (using RPL_NAMREPLY), which MUST include the user joining.
Note that this message accepts a special argument ("0"), which is a special request to leave all channels the user is currently a member of.  The server will process this message as if the user had sent a PART command (See Section 3.2.2) for each channel he is a member of.
