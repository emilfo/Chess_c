#include "stdio.h"
#include "defs.h"

#define MOVE(f,t,ca,pro,fl) ( (f) | ((t) << 7) | ( (ca) << 14 ) | ( (pro) << 20 ) | (fl))
#define SQOFFBOARD(sq) (GetFile[(sq)]==OFFBOARD)

const int LoopSlidePiece[8] = { wB, wR, wQ, 0, bB, bR, bQ, 0};
const int LoopSlideIndex[2] = { 0, 4};

const int LoopNonSlidePiece[6] = { wN, wK, 0, bN, bK, 0};
const int LoopNonSlideIndex[2] = { 0, 3};

const int PieceDir[13][8] = {
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 }
};

const int NumDir[13] = {
 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};

const int VictimScore[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };
static int MvvLvaScores[13][13];

//init most valuable victim, least valueable target
void InitMvvLva() {
	int attacker, victim;

	for (attacker = wP; attacker <= bK; attacker++) {
		for (victim = wP; victim <= bK; victim++) {
			MvvLvaScores[victim][attacker] = VictimScore[victim] +
											6 - (VictimScore[attacker] / 100);
		}
	}
}

int MoveExists(S_BOARD *pos, const int move) {

	S_MOVELIST list[1];
    GenerateAllMoves(pos,list);

    int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {

        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        TakeMove(pos);
		if(list->moves[MoveNum].move == move) {
			return TRUE;
		}
    }
	return FALSE;
}

static void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
	ASSERT(SqOnBoard(FROM(move)));
	ASSERT(SqOnBoard(TO(move)));

	list->moves[list->count].move = move;

	if (pos->searchKillers[0][pos->ply] == move) {
		list->moves[list->count].score = 900000;

	} else if (pos->searchKillers[1][pos->ply]) {
		list->moves[list->count].score = 800000;
	} else {
		list->moves[list->count].score = pos->searchHistory[pos->pieces[FROM(move)]][TO(move)];
	}
	list->count ++;
}

static void AddCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
	ASSERT(SqOnBoard(FROM(move)));
	ASSERT(SqOnBoard(TO(move)));
	ASSERT(PieceValid(CAPTURED(move)));

	list->moves[list->count].move = move;
	list->moves[list->count].score =
				MvvLvaScores[CAPTURED(move)][pos->pieces[FROM(move)]] + 1000000;
	list->count ++;
}

static void AddEnPassantMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
	checkBoard(pos);
	ASSERT(SqOnBoard(FROM(move)));
	ASSERT(SqOnBoard(TO(move)));

	list->moves[list->count].move = move;
	list->moves[list->count].score = 105 + 1000000;
	list->count ++;
}

static void AddWhitePawnCapMove( const S_BOARD *pos, const int from,
							const int to, const int cap, S_MOVELIST *list ) {

	ASSERT(PieceValidEmpty(cap));
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));

	if(GetRank[from] == RANK_7) {
		AddCaptureMove(pos, MOVE(from,to,cap,wQ,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,wR,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,wB,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,wN,0), list);
	} else {
		AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
	}
}

static void AddWhitePawnMove( const S_BOARD *pos, const int from, const int to, S_MOVELIST *list ) {

	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));

	if(GetRank[from] == RANK_7) {
		AddQuietMove(pos, MOVE(from,to,EMPTY,wQ,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,wR,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,wB,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,wN,0), list);
	} else {
		AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
	}
}

static void AddBlackPawnCapMove( const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list ) {

	ASSERT(PieceValidEmpty(cap));
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));

	if(GetRank[from] == RANK_2) {
		AddCaptureMove(pos, MOVE(from,to,cap,bQ,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,bR,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,bB,0), list);
		AddCaptureMove(pos, MOVE(from,to,cap,bN,0), list);
	} else {
		AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
	}
}

static void AddBlackPawnMove( const S_BOARD *pos, const int from, const int to, S_MOVELIST *list ) {

	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));

	if(GetRank[from] == RANK_2) {
		AddQuietMove(pos, MOVE(from,to,EMPTY,bQ,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,bR,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,bB,0), list);
		AddQuietMove(pos, MOVE(from,to,EMPTY,bN,0), list);
	} else {
		AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
	}
}

void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list) {

	ASSERT(checkBoard(pos));
	list->count = 0;

	int piece = EMPTY;
	int side = pos->side;
	int sq = 0;
	int tmpSq = 0;
	int pieceNum = 0;

	int dir = 0
;	int i = 0;
	int pieceIndex = 0;

	if(side == WHITE) {
		//pawn moves
		for (pieceNum = 0; pieceNum < pos->pceNum[wP]; pieceNum++) {
			sq = pos->pList[wP][pieceNum];
			ASSERT(SqOnBoard(sq));


			if(pos->pieces[sq + 10] == EMPTY) {
				AddWhitePawnMove(pos, sq, (sq+10), list);
				if(GetRank[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY) {
					AddQuietMove(pos, MOVE(sq, (sq+20), EMPTY, EMPTY, MFLAGPS), list);
				}
			}

			if(!SQOFFBOARD(sq + 9) && PieceColor[pos->pieces[sq + 9]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, (sq+9), pos->pieces[sq + 9], list);
			}
			if(!SQOFFBOARD(sq + 11) && PieceColor[pos->pieces[sq + 11]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, (sq+11), pos->pieces[sq + 11], list);
			}

			if(pos->enPassantSQ != NO_SQ) {
				if(sq+9 == pos->enPassantSQ) {
					AddEnPassantMove(pos, MOVE(sq, (sq+9), EMPTY, EMPTY, MFLAGEP), list);
				}

				if(sq+11 == pos->enPassantSQ) {
					AddEnPassantMove(pos, MOVE(sq, (sq+11), EMPTY, EMPTY, MFLAGEP), list);
				}
			}

		}

		//castling
		if(pos->castlePerm & WKCA) {
			if(pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
				if(!SqAttacked(E1,BLACK,pos) && !SqAttacked(F1,BLACK,pos)) {
					AddQuietMove(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}

		if(pos->castlePerm & WQCA) {
			if(pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY) {
				if(!SqAttacked(E1,BLACK,pos) && !SqAttacked(D1,BLACK,pos)) {
					AddQuietMove(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}


	} else { //BLACK
		//pawn moves
		for (pieceNum = 0; pieceNum < pos->pceNum[bP]; pieceNum++) {
			sq = pos->pList[bP][pieceNum];
			ASSERT(SqOnBoard(sq));


			if(pos->pieces[sq - 10] == EMPTY) {
				AddBlackPawnMove(pos, sq, (sq-10), list);
				if(GetRank[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY) {
					AddQuietMove(pos, MOVE(sq, (sq-20), EMPTY, EMPTY, MFLAGPS), list);
				}
			}

			if(!SQOFFBOARD(sq - 9) && PieceColor[pos->pieces[sq - 9]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, (sq-9), pos->pieces[sq - 9], list);
			}
			if(!SQOFFBOARD(sq - 11) && PieceColor[pos->pieces[sq - 11]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, (sq-11), pos->pieces[sq - 11], list);
			}

			if(pos->enPassantSQ != NO_SQ) {
				if(sq-9 == pos->enPassantSQ) {
					AddEnPassantMove(pos, MOVE(sq, (sq-9), EMPTY, EMPTY, MFLAGEP), list);
				}

				if(sq-11 == pos->enPassantSQ) {
					AddEnPassantMove(pos, MOVE(sq, (sq-11), EMPTY, EMPTY, MFLAGEP), list);
				}
			}

		}

		if(pos->castlePerm & BKCA) {
			if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
				if(!SqAttacked(E8,WHITE,pos) && !SqAttacked(F8,WHITE,pos)) {
					AddQuietMove(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}

		if(pos->castlePerm & BQCA) {
			if(pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY) {
				if(!SqAttacked(E8,WHITE,pos) && !SqAttacked(D8,WHITE,pos)) {
					AddQuietMove(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}
	}

	/*Sliding pieces*/
	pieceIndex = LoopSlideIndex[side];
	piece = LoopSlidePiece[pieceIndex++];

	while(piece != 0) {
		ASSERT(PieceValid(piece));

		//looping through all the pieces on the board that is the same as piece
		for(pieceNum = 0; pieceNum < pos->pceNum[piece]; pieceNum++) {
			sq = pos->pList[piece][pieceNum];
			ASSERT(SqOnBoard(sq));

			//looping through all the direction the piece can go
			for(i = 0; i < NumDir[piece]; i++) {
				dir = PieceDir[piece][i];
				tmpSq = sq + dir;

				//going in that direction until the square isnt empty
				while(pos->pieces[tmpSq] == EMPTY) {
					AddQuietMove(pos, MOVE(sq, tmpSq, EMPTY, EMPTY, 0), list);
					tmpSq += dir;
				}

				if(!SQOFFBOARD(tmpSq) ) {
					if (PieceColor[pos->pieces[tmpSq]] == (side^1)) {
						//Capture move
						AddCaptureMove(pos, MOVE(sq, tmpSq, pos->pieces[tmpSq], EMPTY, 0), list);
					}
				}

			}

		}


		piece = LoopSlidePiece[pieceIndex++];
	}

/*Nonsliding pieces */
	pieceIndex = LoopNonSlideIndex[side];
	piece = LoopNonSlidePiece[pieceIndex++];

	while(piece != 0) {
		ASSERT(PieceValid(piece));

		for(pieceNum = 0; pieceNum < pos->pceNum[piece]; pieceNum++) {
			sq = pos->pList[piece][pieceNum];
			ASSERT(SqOnBoard(sq));

			for(i = 0; i < NumDir[piece]; i++ ) {
				dir = PieceDir[piece][i];
				tmpSq = sq + dir;

				if(!SQOFFBOARD(tmpSq)) {
					if(pos->pieces[tmpSq] == EMPTY ) {
						//quiet move
						AddQuietMove(pos, MOVE(sq, tmpSq, EMPTY, EMPTY, 0), list);
					} else if (PieceColor[pos->pieces[tmpSq]] == (side^1)) {
						//Capture move
						AddCaptureMove(pos, MOVE(sq, tmpSq, pos->pieces[tmpSq], EMPTY, 0), list);
					}
				}
			}
		}
		piece = LoopNonSlidePiece[pieceIndex++];
	}
}


void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list) {

	ASSERT(checkBoard(pos));
	list->count = 0;

	int piece = EMPTY;
	int side = pos->side;
	int sq = 0;
	int tmpSq = 0;
	int pieceNum = 0;

	int dir = 0;
	int i = 0;
	int pieceIndex = 0;

	if(side == WHITE) {
		//pawn moves
		for (pieceNum = 0; pieceNum < pos->pceNum[wP]; pieceNum++) {
			sq = pos->pList[wP][pieceNum];
			ASSERT(SqOnBoard(sq));

			if(!SQOFFBOARD(sq + 9) && PieceColor[pos->pieces[sq + 9]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, (sq+9), pos->pieces[sq + 9], list);
			}
			if(!SQOFFBOARD(sq + 11) && PieceColor[pos->pieces[sq + 11]] == BLACK) {
				AddWhitePawnCapMove(pos, sq, (sq+11), pos->pieces[sq + 11], list);
			}

			if(pos->enPassantSQ != NO_SQ) {
				if(sq+9 == pos->enPassantSQ) {
					AddEnPassantMove(pos, MOVE(sq, (sq+9), EMPTY, EMPTY, MFLAGEP), list);
				}

				if(sq+11 == pos->enPassantSQ) {
					AddEnPassantMove(pos, MOVE(sq, (sq+11), EMPTY, EMPTY, MFLAGEP), list);
				}
			}

		}

	} else { //BLACK
		//pawn moves
		for (pieceNum = 0; pieceNum < pos->pceNum[bP]; pieceNum++) {
			sq = pos->pList[bP][pieceNum];
			ASSERT(SqOnBoard(sq));

			if(!SQOFFBOARD(sq - 9) && PieceColor[pos->pieces[sq - 9]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, (sq-9), pos->pieces[sq - 9], list);
			}
			if(!SQOFFBOARD(sq - 11) && PieceColor[pos->pieces[sq - 11]] == WHITE) {
				AddBlackPawnCapMove(pos, sq, (sq-11), pos->pieces[sq - 11], list);
			}

			if(pos->enPassantSQ != NO_SQ) {
				if(sq-9 == pos->enPassantSQ) {
					AddEnPassantMove(pos, MOVE(sq, (sq-9), EMPTY, EMPTY, MFLAGEP), list);
				}

				if(sq-11 == pos->enPassantSQ) {
					AddEnPassantMove(pos, MOVE(sq, (sq-11), EMPTY, EMPTY, MFLAGEP), list);
				}
			}

		}
	}

	/*Sliding pieces*/
	pieceIndex = LoopSlideIndex[side];
	piece = LoopSlidePiece[pieceIndex++];

	while(piece != 0) {
		ASSERT(PieceValid(piece));

		//looping through all the pieces on the board that is the same as piece
		for(pieceNum = 0; pieceNum < pos->pceNum[piece]; pieceNum++) {
			sq = pos->pList[piece][pieceNum];
			ASSERT(SqOnBoard(sq));

			//looping through all the direction the piece can go
			for(i = 0; i < NumDir[piece]; i++) {
				dir = PieceDir[piece][i];
				tmpSq = sq + dir;

				//going in that direction until the square isnt empty
				while(pos->pieces[tmpSq] == EMPTY) {
					tmpSq += dir;
				}

				if(!SQOFFBOARD(tmpSq) ) {
					if (PieceColor[pos->pieces[tmpSq]] == (side^1)) {
						//Capture move
						AddCaptureMove(pos, MOVE(sq, tmpSq, pos->pieces[tmpSq], EMPTY, 0), list);
					}
				}

			}

		}


		piece = LoopSlidePiece[pieceIndex++];
	}

/*Nonsliding pieces */
	pieceIndex = LoopNonSlideIndex[side];
	piece = LoopNonSlidePiece[pieceIndex++];

	while(piece != 0) {
		ASSERT(PieceValid(piece));

		for(pieceNum = 0; pieceNum < pos->pceNum[piece]; pieceNum++) {
			sq = pos->pList[piece][pieceNum];
			ASSERT(SqOnBoard(sq));

			for(i = 0; i < NumDir[piece]; i++ ) {
				dir = PieceDir[piece][i];
				tmpSq = sq + dir;

				if(!SQOFFBOARD(tmpSq)) {
					if(pos->pieces[tmpSq] == EMPTY ) {
					} else if (PieceColor[pos->pieces[tmpSq]] == (side^1)) {
						//Capture move
						AddCaptureMove(pos, MOVE(sq, tmpSq, pos->pieces[tmpSq], EMPTY, 0), list);
					}
				}
			}
		}
		piece = LoopNonSlidePiece[pieceIndex++];
	}
}

