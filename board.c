#include "stdio.h"
#include "defs.h"

//resets the board to startposition
void ResetBoard (S_BOARD *pos) {
	int i = 0;

			//sets all squares to offboard
	for(i = 0; i < BRD_SQ_NUM; i++) {
		pos->pieces[i] = OFFBOARD;
	}

	//sets actual board to empty
	for(i = 0; i < 64; i++) {
		pos->pieces[SQ120(i)] = EMPTY;
	}

	//sets all pieces to 0, big, major, min, and pawns
	for(i = 0; i < 3; i++) {
		pos->bigPce[i] = 0;
		pos->majPce[i] = 0;
		pos->minPce[i] = 0;
		pos->pawns[i] = 0ULL;
	}

	//resets pieceNumber to zero
	for(i = 0; i < 13; i++) {
		pos->pceNum[i] = 0;
	}

	//Kings arent on any squares
	pos->kingSQ[WHITE] = pos->kingSQ[BLACK] = NO_SQ;

    //not a valid side
	pos->side = BOTH;
	//no enPassant
	pos->enPassantSQ = NO_SQ;
	//no moves taken
	pos->fiftyMove = 0;

	//no ply
	pos->ply = 0;
	pos->hisPly = 0;

	//no castling
	pos->castlePerm = 0;

	//no posKey
	pos->posKey = 0ULL;
}

