#include "defs.h"

int SqOnBoard(const int sq) {
	return GetFile[sq]==OFFBOARD ? FALSE : TRUE;
}

int SideValid(const int side) {
	return (side==WHITE || side == BLACK) ? TRUE : FALSE;
}

int FileRankValid(const int fr) {
	return (fr >= 0 && fr <= 7) ? TRUE : FALSE;
}

int PieceValidEmpty(const int pce) {
	return (pce >= EMPTY && pce <= bK) ? TRUE : FALSE;
}

int PieceValid(const int pce) {
	return (pce >= wP && pce <= bK) ? TRUE : FALSE;
}