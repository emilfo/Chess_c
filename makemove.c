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


static void addPiece(const int sq, S_BOARD *pos, const int piece) {
	ASSERT(pieceValid(piece))
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
	int targetPceNum = FALSE
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
			pos->pList[piece][index] = to;
#ifdef DEBUG
			targetPceNum = TRUE;
#endif
			break;
		}
	}

	ASSERT(targetPceNum);
}