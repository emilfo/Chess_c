#include "defs.h"
#include "stdio.h"

const int KnDir[8] = { -8, -19,	-21, -12, 8, 19, 21, 12 };
const int RkDir[4] = { -1, -10,	1, 10 };
const int BiDir[4] = { -9, -11, 11, 9 };
const int KiDir[8] = { -1, -10,	1, 10, -9, -11, 11, 9 };

int SqAttacked(const int sq, const int side, const S_BOARD *pos) {
	int piece;
	int tmpSQ;
	int dir;
	int i;

	ASSERT(SqOnBoard(sq));
	ASSERT(SideValid(side));
	ASSERT(checkBoard(pos));

	//pawn attacks
	if(side == WHITE) {
		if(pos->pieces[sq-11] == wP || pos->pieces[sq-9] == wP)  return TRUE;
	} else { //BLACK
		if(pos->pieces[sq+11] == bP || pos->pieces[sq+9] == bP) return TRUE;
	}

	//knight attacks
	for(i = 0; i < 8; i++) {
		piece = pos->pieces[ sq + KnDir[i] ];
		if(IsKn(piece) && PieceColor[piece]==side) return TRUE;
	}

	//king attacks
	for(i = 0; i < 8; i++) {
		piece = pos->pieces[ sq + KiDir[i] ];
		if(IsKi(piece) && PieceColor[piece]==side) return TRUE;
	}

	//rook and Queen attacks
	for(i = 0; i < 4; i++) {
		dir = RkDir[i];
		tmpSQ = sq + dir;
		piece = pos->pieces[tmpSQ];

		while(piece != OFFBOARD) {
			if(piece != EMPTY) {
				if(IsRQ(piece) && PieceColor[piece] == side) return TRUE;
				break;
			}

			tmpSQ += dir;
			piece = pos->pieces[tmpSQ];
		}
	}

	//bishop and queen attacks
	for(i = 0; i < 4; i++) {
		dir = BiDir[i];
		tmpSQ = sq + dir;
		piece = pos->pieces[tmpSQ];

		while(piece != OFFBOARD) {
			if(piece != EMPTY) {
				if(IsBQ(piece) && PieceColor[piece] == side) return TRUE;
				break;
			}

			tmpSQ += dir;
			piece = pos->pieces[tmpSQ];
		}
	}

	return FALSE;
}