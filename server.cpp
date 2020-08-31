#include <sys/socket.h>
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#define MAX_BUF_SIZE 4
#define ROW 3
#define COL 3
using namespace std;

//outputs error message then halts the program
void error_mesg(string message){
	cout << message << endl;
	exit(0);
}

/*check_winner() returns 'X' 'O' or '_'
'X' = player wins
'O' = server wins
'_' = no winner
*/
char check_winner(char board[ROW][COL]){
	char winner = '_';

	//check for diagonal win
	if(board[0][0] != '_' && board[0][0] == board[1][1] && board[1][1] == board[2][2]){
		winner = board[0][0];
	}
	else if(board[0][2] != '_' && board[0][2] == board[1][1] && board[1][1] == board[2][0]){
		winner = board[0][2];
	}

	//check for horizontal win
	for(int i = 0; i < ROW; i++){
		if(board[i][0] != '_'){
			if(board[i][0] == board[i][1] && board[i][1] == board[i][2]){
				winner = board[i][0];
				break;
			}
		}
	}

	//check for vertical win
	for(int j = 0; j < COL; j++){
		if(board[0][j] != '_'){
			if(board[0][j] == board[1][j] && board[1][j] == board[2][j]){
				winner = board[0][j];
				break;
			}
		}
	}

	return winner;
}

/*make_move() will randomly make a move.
It will try repeatedly until it makes a valid move.
*/
void make_move(char board[3][3], char move[3]){
	
	bool valid_move = false;
	while(!valid_move){
		int x = rand() % 3;
		int y = rand() % 3;
		//once a valid move is found apply it to the board
		//and copy the move into move[].
		if(board[x][y] == '_'){
			board[x][y] = 'O';
			//(char)'0' + int(x) = char(x)
			move[0] = '0' + x;
			move[1] = '0' + y;
			move[2] = '\0';
			valid_move = true;
		}
	}
}

int main(int argc, char** argv){

	srand(time(0)); //seed the rand function
	int socket_fd, new_socket_fd, client_len;
	struct sockaddr_in server_addr, client_addr;

	if(argc < 2){
		error_mesg("Usage: ./Server port");
	}

	//create the socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0){
		error_mesg("Error opening socket");
	}
	cout << "Socket created." << endl;

	//initialize the server struct
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1])); //htons is "host to network" byte order
	server_addr.sin_addr.s_addr = INADDR_ANY;	

	//bind to the socket
	if(bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
		error_mesg("Error on binding");
	}
	cout << "Bind completed." << endl;

	//listen on the socket
	listen(socket_fd, 1);
	client_len = sizeof(client_addr);
	
	//loop for accepting new connections
	while(true){
		
		//accept connection
		cout << "Waiting for incoming connection from a player..." << endl;
		new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, (unsigned int *)&client_len);
		if(new_socket_fd < 0){
			error_mesg("Error on accept");
		}
		cout << "Connection accepted." << endl;

		int turn_count = 0;
		bool game_over = false;
		char winner = '_';
		char move[3];
		char buffer[MAX_BUF_SIZE];
		char board[ROW][COL] = {{'_', '_', '_'},
								{'_', '_', '_'},
								{'_', '_', '_'}};

		//main game loop, plays one game of tic-tac-toe
		while(!game_over){
			
			//reset buffer and move after each turn.
			memset(buffer, 0, MAX_BUF_SIZE);
			memset(move, 0, 3);

			//get move from client
			if(recv(new_socket_fd, buffer, MAX_BUF_SIZE - 1, 0) < 0){
				error_mesg("Error on recv.");
			}
		
			cout << "Move received from player (" << buffer[0]
			<< " " << buffer[1] << ")." << endl;
			
			//apply the client's move to the board
			if(strlen(buffer) == 2){
				//(char)x - '0' will convert (char)x to (int)x
				int x = buffer[0] - '0';
				int y = buffer[1] - '0';
				board[x][y] = 'X';
			}
			else{
				error_mesg("Invalid move by player.");
			}

			//check for winner before trying to make a move
			//turn_count checks for a full game board
			winner = check_winner(board);
			if(++turn_count == 5 || winner != '_'){
				game_over = true;
				break;
			}

			//server makes its move, sends it.
			make_move(board, move);
			
			cout << "Move sent to client(" << move[0] << " "
			<< move[1] << ")." << endl;

			if(send(new_socket_fd, move, strlen(move), 0) < 0){
				error_mesg("Error on send.");
			}

			winner = check_winner(board);
			if(winner != '_'){
				game_over = true;
			}

		}
		cout << "Connection with client ended." << endl;
	}

	close(new_socket_fd);
	cout << "Connection closed." << endl;
	return 0;
}

