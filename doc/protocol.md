# TarotClub network protocol

## Transport layer and header

TCP/IP on port 4269

## Tarot Game Notation

FIXME: describe the JSON deal format

## Protocol

### Login Server

TarotClub server is using HTTPS for login and for important account actions (buy/sell/transfer items). When the client logins to the server over HTTPS the server generates a long (64-128 bytes) random session id that is stored in the database (RAM only DB such as XXX). Also generate a temporary one-time authentication token (long random value) for the client's TCP connection.

The reason for using HTTPS is to help stop MITM attacks from stealing the users in game assets.


### Lobby connection

Client perform a TCP connection to the game server.

Client receive a request for user login with several parameters. uuid: assigned uuid of the new client

{
	"cmd": "RequestLogin",
	"uuid": 766
} 

3. Server receive connection parameters

{
	"cmd": "ReplyLogin",
	"nickname": "Belegar"
}

4. Client reseive the list of available tables

{
     "cmd": "AccessGranted"
	 "tables": [
		{ "name": "Table 1", "uuid", 890},
		{ "name": "Table 2", "uuid", 891}
	]
}

### List of players, status and event

{
	"cmd": "PlayersList", 
	"players": [
		{ "uuid": 37, "nickname": "Belegar", "table": 0, "event": "JoinTable" }
		{ }
	]
}

### Change nickame

Client can ask to the server to change the identity

{
	"cmd": "RequestChangeNickname",
	"nickname": "Zouma"
}

### Chat message

All the stats messages are always sent to the lobby.
Source indicate the UUID of the source of the message (player, Table or Lobby).
Target indicate the destination UUID of the message.

{
	"cmd": "ChatMessage", 
	"source": 27,
	"target": 789
}

### Join/Quit a table

Client send a request to the lobby with the specified table number to join

{
	"cmd": "RequestJoinTable",
	"table_id": 27
}

If there is at least one free slot around the table, the server replies:

{
	"cmd": "ReplyJoinTable",
	"table_id": 27,
	"size": 4,
	"players": [
		{ "place": "South", uuid": 367 },
	]
}

The server sends some indication regarding the players around the table. The client is in charge to retrieve its own place 
thanks to the array of players.

Finally, the client sends an acknowledge:

{
	"cmd": "Ack",
	"step": "JoinTable"
}

in case of error, the following data is sent by the server:

{
	"cmd": "Error",
	"code": 7,
	"reason": "Table is full"
}

Client can quit a table

{
	"cmd": "RequestQuitTable",
	"table_id": 98987
}

### Deal type

{
	"type": "Random", // or "Numbered", "Custom"
	"file": "./mygame.json",  // File name (local to the server)
	"seed": 78895687790009  // deal number
}

### New game

A dedicated command must be sent to start a new game. This command can be issued from the administrator of the table or the server itself, depending 
of the game parameters.

Client (admin) --> Server (table)

{
	"cmd": "RequestNewGame",
	"mode": "QuickDeal", // or "SimpleTournament", "Custom"
	"deals": [
		... // list of deal type
	]
}

Server (table) --> Client (all)

{
	"cmd": "NewGame",
	"mode": "QuickDeal",
	"turns": 4
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Ready"
}

### New deal

Server (table) --> Client

{
	"cmd": "NewDeal",
	"cards": "T-02;S-11; ...."
}


Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "NewDeal"
}

### Tarot game start sequence (bid and discard)

Server (table) --> Client

{
	"cmd": "RequestBid",
	"place": "South",		// Current player
	"contract": "Guard"		// highest previous bid announced
}

Client  --> Server (table)

{
	"cmd": "Bid",
	"contract": "Pass",
	"slam": false			// Set to true to announce a slam
}

Then the server broadcast to the table the announced bid for this player

Server (table) --> Client (all)

{
	"cmd": "ShowBid",
	"place": "South",
	"contract": "Pass",
	"slam": false
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Bid"
}

If all players have passed :
Server (table) --> Client (all)

{
	"cmd": "AllPassed",
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "AllPassed"
}

Otherwise, show the dog if allowed:

Server (table) --> Client (all)

{
	"cmd": "ShowDog",
	"dog": "T02S11...."
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Dog"
}

Then the server sends to the taker the discard order, if required:

Server (table) --> Client (taker)

{
	"cmd": "BuildDiscard",
}

Client (taker) --> Server (table)

{
	"cmd": "Discard",
	"discard": "T02S11...."
}

Then the server starts the deal

Server (table) --> Client (all)

{
	"cmd": "StartDeal",
	"first_player": "South",
    "taker": "North",
    "contract": "Guard",
    "slam": false
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Start"
}

### Tarot game sequence

For the first turn where players can decalre a handle

Server (table) --> Client (player)

{
	"cmd": "AskForHandle",
}

The Client will then always answer with the handle, if any, or any invalid deck (an empty deck is fine)

Client (player) --> Server (table)

{
	"cmd": "Handle",
	"handle": "T02S11...." // or "handle": "" if no handle to set
}

In a case of an handle, it is shown to all players of the table:

Server (table) --> Client (all)

{
	"cmd": "Handle",
	"place": "South",
	"handle": "T02S11...."
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Handle"
}

After that, for each game turn, the sequence is the same. The server ask to play a card:

Server (table) --> Client (all)

{
	"cmd": "PlayCard",
	"dog": "T02S11...."
}

Client (player) --> Server (table)

{
	"cmd": "Card",
	"card": "T02"
}

Server (table) --> Client (all)

{
	"cmd": "PlayCard",
	"place": "North",
	"card": "T02"
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Card"
}

At the end of the trick, the server sends:

Server (table) --> Client (all)

{
	"cmd": "EndOfTrick",
	"place": "North" 		// winner of the trick 
}

Client (all) --> Server (table)

{
	"cmd": "Ack",
	"step": "Trick"
}

At the end of the deal:

Server (table) --> Client (all)

{
	"cmd": "EndOfDeal",
	
}


