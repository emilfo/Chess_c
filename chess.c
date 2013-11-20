#include "stdio.h"
#include "defs.h"
#include "stdlib.h"

#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define FEN4 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
int main () {
	AllInit();

	S_BOARD board[1];

	parseFEN(FEN4, board);
	//asserting that all the pieces are correct
	ASSERT(checkBoard(board));

 	//force assert error
	//board->posKey ^=SideKey;
	//ASSERT(checkBoard(board));

	int move = 0;

	int from = A2;
	int to = A4;
	int cap = wR;
	int prom = bQ;

	move = ( (from) | (to<<7) | (cap << 14) | (prom << 20) );

	printf("%s\n", printSquare(from));
	printf("%s\n", printSquare(to));
	printf("%s", printMove(move));

	return 0;
}