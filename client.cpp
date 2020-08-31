#include <sys/socket.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <ctime>
#define ROW 3
#define COL 3
using namespace std;

//prints error message then halts the program
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

/*player_turn() gets the player's move from standard input.
If the move the player gives is invalid, it tries again.
*/
void player_turn(char board[ROW][COL], char move[3]){
	bool valid_move = false;
	int x, y;

	while(!valid_move){
		cout << "Make move (x, y): ";
		if(scanf("%d %d", &x, &y) != 2){
			error_mesg("Invalid input");
		}

		if(board[x][y] != '_'){
			cout << "Invalid move." << endl;
		}
		else{
			board[x][y] = 'X';
			valid_move = true;
		}
	}
	//copy the coordinates as characters into move[]
	//(char)'0' - (int)x = (char)x
	move[0] = '0' + x; 
	move[1] = '0' + y;
	move[2] = '\0';

	return;
}

//Draws the game board according to the example
//run in the prompt.
void draw_board(char board[3][3]){
	for(int i = 0; i < ROW; i++){
		for(int j = 0; j < COL; j++){
			cout << board[i][j] << " ";
		}
		cout << endl;
	}
}

int main(int argc, char** argv){

	if(argc < 3){
		string name = argv[0];
		error_mesg("Usage: " + name + " address port");
	}

	struct sockaddr_in address, server_addr;
	int socket_fd;

	//create the socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0){
		error_mesg("Error making socket");
	}
	cout << "Socket created." << endl;

	//initialize connection with port and address
	memset(&server_addr, '0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	//connect to the server
	if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		error_mesg("Erorr on connect");
	}
	cout << "Connected." << endl;

	// _(underscore) is free space, X is player, O is server
	char board[ROW][COL] = {'_', '_', '_',
						'_', '_', '_',
						'_', '_', '_'};
	int x, y, turn_count = 1;
	bool game_over = false;
	char move[3];
	char winner = '_';
	char buffer[256];

	memset(buffer, 0, 256);
	draw_board(board);

	//main game loop, plays one game of tic-tac-toe
	while(!game_over){
	
		//reset buffer and move after each turn	
		memset(buffer, 0, 256);
		memset(move, 0, 3);

		//make and send move
		player_turn(board, move);
		send(socket_fd, move, strlen(move), 0);

		//check for win before allowing server to make a move
		//turn_count is used to check for a full board
		winner = check_winner(board);
		if(winner != '_' || turn_count++ == 5){
			game_over = true;
			draw_board(board);
			break;
		}

		//receive computer move
		if(recv(socket_fd, buffer, 255, 0) < 0){
			error_mesg("Error on recv.");
		}

		
		//apply server's move to the board
		if(strlen(buffer) == 2){
			//(char)x - '0' will convert (char)x to (int)x
			int x = buffer[0] - '0';
			int y = buffer[1] - '0';
			board[x][y] = 'O';
		}
		else{
			error_mesg("Bad input from server.");
		}

		draw_board(board);
		winner = check_winner(board);
		if(winner != '_'){
			game_over = true;
		}
		
	}

	//output winner	
	if(winner == '_'){
		cout << "Draw." << endl;
	}
	else if(winner == 'X'){
		cout << "You win." << endl;
	}
	else{
		cout << "You lose." << endl;
	}

	//close connection
	close(socket_fd);

	return 0;
	
}

