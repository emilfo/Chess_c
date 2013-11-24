#include "defs.h"
#include "stdio.h"


#define HASH_CA (pos->posKey ^= (CastleKeys[(pos->castlePerm)]))
#define HASH_PCE(piece,sq) (pos->posKey ^= (PieceKeys[(piece)][(sq)]))
#define HASH_SIDE (pos->posKey ^= (SideKey))
#define HASH_EP (pos->posKey ^= (PieceKeys[EMPTY][(pos->enPassantSQ)]))

const int CastlePerm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

static void ClearPiece(const int sq, S_BOARD *pos) {
	ASSERT(SqOnBoard(sq));

	int piece = pos->pieces[sq];

	ASSERT(PieceValid(piece));

	int color = PieceColor[piece];
	int i = 0;
	int targetPceNum = -1;

	//remove piece from posKey
	HASH_PCE(piece,sq);

	pos->pieces[sq] = EMPTY;
	pos->material[color] -= PieceVal[piece];

	//reduces the number of the various arrays the piece was indexed in
	if(PieceBig[piece]) {
		pos->bigPce[color]--;

		if(PieceMaj[piece]) {
			pos->majPce[color]--;
		}
		else {
			pos->minPce[color]--;
		}

	//if it is a pawn, removes it from the bitboard
	} else {
		CLRBIT(pos->pawns[color], SQ64(sq));
		CLRBIT(pos->pawns[BOTH], SQ64(sq));
	}

	//finds the piece in the pList
	for (i = 0; i < pos->pceNum[piece]; i++) {
		if(pos->pList[piece][i] == sq) {
			targetPceNum = i;
			break;
		}
	}

	ASSERT(targetPceNum != -1);

	//reduces the pceNum for the piece we're removing
	pos->pceNum[piece]--;
	//overwrites the piece to be removed with the current last piece in the pList
	pos->pList[piece][targetPceNum] = pos->pList[piece][pos->pceNum[piece]];
}


static void AddPiece(const int sq, S_BOARD *pos, const int piece) {
	ASSERT(PieceValid(piece))
	ASSERT(SqOnBoard(sq));

	int color = PieceColor[piece];
	int i = 0;
	int targetPceNum = -1;

	//adds piece to posKey
	HASH_PCE(piece,sq);

	pos->pieces[sq] = piece;

	pos->material[color] += PieceVal[piece];

	//reduces the number of the various arrays the piece was indexed in
	if(PieceBig[piece]) {
		pos->bigPce[color]++;

		if(PieceMaj[piece]) {
			pos->majPce[color]++;
		}
		else {
			pos->minPce[color]++;
		}

	//if it is a pawn, removes it from the bitboard
	} else {
		SETBIT(pos->pawns[color], SQ64(sq));
		SETBIT(pos->pawns[BOTH], SQ64(sq));
	}

	//adds the piece to the end of pList and increase the number of pieces of this type by 1
	pos->pList[piece][pos->pceNum[piece]++] = sq;
}

static void MovePiece(const int from, const int to, S_BOARD *pos) {
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));

	int i = 0;
	int piece = pos->pieces[from];
	int color = PieceColor[piece];

#ifdef DEBUG
	int targetPceNum = FALSE;
#endif

	//removes the piece and its old square from the posKey and sets the from-square to empty
	HASH_PCE(piece, from);
	pos->pieces[from] = EMPTY;

	//adds the piece and its new square to the posKey and set the to-square to piece
	HASH_PCE(piece, to);
	pos->pieces[to] = piece;

	//if its a pawn, change the bitboard
	if(!PieceBig[piece]) {
		CLRBIT(pos->pawns[color], SQ64(from));
		CLRBIT(pos->pawns[BOTH], SQ64(from));
		SETBIT(pos->pawns[color], SQ64(to));
		SETBIT(pos->pawns[BOTH], SQ64(to));
	}

	for(i = 0; i < pos->pceNum[piece]; i++) {
		if(pos->pList[piece][i] == from) {
			pos->pList[piece][i] = to;
#ifdef DEBUG
			targetPceNum = TRUE;
#endif
			break;
		}
	}

	ASSERT(targetPceNum);
}

int MakeMove(S_BOARD *pos, int move) {

	ASSERT(checkBoard(pos));

	//bitshifts the move to get its values as set up in the macros INT MOVE
	int from = FROM(move);
	int to = TO(move);
	int side = pos->side;

	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	ASSERT(SideValid(side));
	ASSERT(PieceValid(pos->pieces[from]));

	//Stores current the poskey in history 
	pos->history[pos->hisPly].posKey = pos->posKey;

	//check enpassant move
	if(move & MFLAGEP) {
		if(side == WHITE) ClearPiece(to-10,pos);
		else ClearPiece(to+10,pos);
	}
	//check castling move
	else if(move & MFLAGCA) {
		switch (to) {
			case C1:
				MovePiece(A1, D1, pos); break;
			case C8:
				MovePiece(A8, D8, pos); break;
			case G1:
				MovePiece(H1, F1, pos); break;
			case G8:
				MovePiece(H8, F8, pos);break;

			default: ASSERT(FALSE); break;

		}
	}
	//removes the previous enPassantSQ from posKey
	if(pos->enPassantSQ != NO_SQ) HASH_EP;
	//hash out the current castling state from posKey
	HASH_CA;

	pos->history[pos->hisPly].move = move;
    pos->history[pos->hisPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->hisPly].enPassantSQ = pos->enPassantSQ;
    pos->history[pos->hisPly].castlePerm = pos->castlePerm;

    pos->castlePerm &= CastlePerm[from];
    pos->castlePerm &= CastlePerm[to];
    pos->enPassantSQ = NO_SQ;

    //hash in the new castling state to posKey
	HASH_CA;

	pos->fiftyMove++;

	int captured = CAPTURED(move);

	if(captured != EMPTY) {
		ASSERT(PieceValid(captured));
		ClearPiece(to, pos);
		pos->fiftyMove = 0;
	}

	pos->ply++;
	pos->hisPly++;

	if(PiecePawn[pos->pieces[from]]) {
		pos->fiftyMove = 0;

		//if double pawnmove
		if(move & MFLAGPS) {

			if(side == WHITE) {
				pos->enPassantSQ = from + 10;
				ASSERT(GetRank[pos->enPassantSQ] == RANK_3);
			} else {
				pos->enPassantSQ = from - 10;
				ASSERT(GetRank[pos->enPassantSQ] == RANK_6);
			}
			HASH_EP;
		}
	}

	MovePiece(from, to, pos);

	//checks after promoted pawn-move
	int prPiece = PROMOTED(move);
	if(prPiece != EMPTY) {
		ASSERT(PieceValid(prPiece) && !PiecePawn[prPiece] && !PieceKing[prPiece]);
		ClearPiece(to, pos);
		AddPiece(to, pos, prPiece);
	}

	//checks for kingmove and updates the kingsquare
	if(PieceKing[pos->pieces[to]]) {
		pos->kingSQ[pos->side] = to;
	}

	pos->side ^= 1;

	HASH_SIDE;

	ASSERT(checkBoard(pos));

	//checks if the side that moved had their king in check, in that case illegal move
	if(SqAttacked(pos->kingSQ[side], pos->side, pos)) {
		TakeMove(pos);
		return FALSE;
	}

	return TRUE;
}

void TakeMove(S_BOARD *pos) {

	ASSERT(checkBoard(pos));

	pos->hisPly--;
	pos->ply--;

	int move = pos->history[pos->hisPly].move;
	int from = FROM(move);
	int to = TO(move);

	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));

	if(pos->enPassantSQ != NO_SQ) HASH_EP;
	HASH_CA;

	pos->fiftyMove = pos->history[pos->hisPly].fiftyMove;
    pos->enPassantSQ = pos->history[pos->hisPly].enPassantSQ;
    pos->castlePerm = pos->history[pos->hisPly].castlePerm;

    if(pos->enPassantSQ != NO_SQ) HASH_EP;
    HASH_CA;

    pos->side ^= 1;
    HASH_SIDE;

    //if enpassant capture add in the pieces
    if(move & MFLAGEP) {
		if(pos->side == WHITE) AddPiece(to-10, pos, bP);
		else AddPiece(to+10, pos, wP);
	}

	//if castling move move the rook back in position
	else if(move & MFLAGCA) {
		switch (to) {
			case C1:
				MovePiece(D1, A1, pos); break;
			case C8:
				MovePiece(D8, A8, pos); break;
			case G1:
				MovePiece(F1, H1, pos); break;
			case G8:
				MovePiece(F8, H8, pos);break;

			default: ASSERT(FALSE); break;

		}
	}
	//redo move before checking for capture
	MovePiece(to, from, pos);

	//update kingsq
	if(PieceKing[pos->pieces[from]]) {
		pos->kingSQ[pos->side] = from;
	}

	int captured = CAPTURED(move);

	if(captured != EMPTY) {
		ASSERT(PieceValid(captured));
		AddPiece(to, pos, captured);
	}

	int prPiece = PROMOTED(move);
	if(prPiece != EMPTY) {
		ASSERT(PieceValid(prPiece) && !PiecePawn[prPiece] && !PieceKing[prPiece]);
		ClearPiece(from, pos);
		AddPiece(from, pos, (PieceColor[prPiece] == WHITE ? wP : bP));
	}

	ASSERT(checkBoard(pos));

}