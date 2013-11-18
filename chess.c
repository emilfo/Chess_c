#include "stdio.h"
#include "defs.h"
#include "stdlib.h"
#define FEN1 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN2 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"

int main () {
	AllInit();

	S_BOARD board[1];

	parseFEN(FEN1, board);
	printBoard(board);
	parseFEN(FEN2, board);
	printBoard(board);


	return 0;
}