#include "stdio.h"
#include "defs.h"

U64 GeneratePosKey(const S_BOARD *pos) {

	int sq = 0;
	U64 finalKey = 0;
	int piece = EMPTY;
	
	// loop through all pieces and adds them to the hash
	for(sq = 0; sq < BRD_SQ_NUM; ++sq) {
		piece = pos->pieces[sq];
		if(piece!=NO_SQ && piece!=EMPTY && piece !=OFFBOARD) {
			printf("%d",piece);
			ASSERT(piece>=wP && piece<=bK); //making sure its actually is a piece
			finalKey ^= PieceKeys[piece][sq];
		}		
	}
	
	//adds which turn to key
	if(pos->side == WHITE) {
		finalKey ^= SideKey;
	}
		
	//adds enpassant to key
	if(pos->enPassantSQ != NO_SQ) {
		ASSERT(pos->enPassantSQ>=0 && pos->enPassantSQ<BRD_SQ_NUM);
		finalKey ^= PieceKeys[EMPTY][pos->enPassantSQ];
	}
	

	ASSERT(pos->castlePerm>=0 && pos->castlePerm<=15);
	//adding castlekey
	finalKey ^= CastleKeys[pos->castlePerm];
	
	return finalKey;
}