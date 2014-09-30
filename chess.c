#include "stdio.h"
#include "defs.h"
#include "stdlib.h"

#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define FEN4 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

#define PAWNMOVESW "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define PAWNMOVESB "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"
#define KNIGHTSKINGS "5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1"
#define ROOKS "6k1/8/5r2/8/1nR5/5N2/8/6K1 b - - 0 1"
#define QUEENS "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 b - - 0 1 "
#define BISHOPS "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 b - - 0 1 "
#define CASTLE1 "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
#define CASTLE2 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

#define WAC1 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"

int main () {
	AllInit();

	S_BOARD board[1];
	S_SEARCHINFO info[1];
	parseFEN(WAC1, board);

	char input[6];
	int Move = NOMOVE;
	int PvNum = 0;
	int Max = 0;

	while(TRUE) {
		printBoard(board);
		printf("Please enter a move > ");
		fgets(input, 6, stdin);
		
		if(input[0]=='q') {
			break;
		} else if(input[0]=='t') {
			TakeMove(board);			
		} else if(input[0]=='s') {
			info->depth = 4;
			SearchPosition(board, info);
		} else {
			Move = ParseMove(input, board);
			
			if(Move != NOMOVE) {
				StorePvMove(board, Move);
				MakeMove(board,Move);
			} else {
				printf("Move Not Parsed:%s\n",input);
			}
		}	
		
		fflush(stdin);
	}

	
	return 0;
}
