Josh Williams
CS371
Individual project (Server-client Tic-Tac-Toe)

This project consists of the following files:
README (this file)
client.cpp (client code)
server.cpp (server code)
Makefile (compilation file)

####SETUP####
Run make to compile the source files into Client and Server programs.
or simply do g++ client.cpp, g++ server.cpp

This project is meant to run on two different machines on the same network.

First run the server program on the first machine using
./Server port_number
ex. ./Server 12345

Next, run  the client program on a different machine (or the same machine).
./Client ip_address port_number
ex. ./Client 127.0.0.1 12345

#############

#####PLAYING THE GAME#####
Following the example in the assignment prompt, the player plays as X,
the server plays as O and the player always goes first.

The game board is displayed using 
'_'(underscore) to represent an empty space
'X' for the player's move
'O' for the server's move

The client program will display an empty game board and ask for input.
Input is in the form of 2 integers i,j: 0 <= i,j <= 2
The coordinates correspond to the row and column (respectively) of the game
board you wish to apply your move (0 indexed).
For example: move 0 1 will place an X in the 0th row, 1st column.

ex. Make move(x y): 0 1 ->
_ X _
_ _ _
_ _ _

The goal of the game is to get three X's in a row. When this is achieved
or when no more moves are possible, the game will end and print out the winner.
The client then closes the connection with the server. The server will then
wait for another connection from a client program.

###########################

####CLIENT#####
The client asks for input from the user using standard input.
It expects two integers. If the input is out of the range of the board
or corresponds to a non-empty space, then it will ask for input again.
If the input is not two integers, the program will end.

The client sends only 3 bytes on every turn. The move[] array holds
the two coordinates that is the player's move as characters and a '\0' null character.
##############

####SERVER####
The server makes it moves randomly until a valid move is made. It does not
try to win. 

The server sends its moves the same way the client does, as a three byte c-string.
#############
