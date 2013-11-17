#include "stdio.h"
#include "defs.h"
#include "stdlib.h"
#define FEN1 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

int main () {
	AllInit();

	S_BOARD board[1];

	parseFEN(FEN1, board);
	printBoard(board);


	return 0;
}