#include "stdio.h"
#include "defs.h"

#define MOVE(f,t,ca,pro,fl) ( (f) | ((t) << 7) | ( (ca) << 14 ) | ( (pro) << 20 ) | (fl))
#define SQOFFBOARD(sq) (GetFile[(sq)]==OFFBOARD)

int LoopSlidePiece[8] = { wB, wR, wQ, 0, bB, bR, bQ, 0};
int LoopSlideIndex[2] = { 0, 4};

int LoopNonSlidePiece[6] = { wN, wK, 0, bN, bK, 0};
int LoopNonSlideIndex[2] = { 0, 3};

int PieceDir[13][8] = {
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

int NumDir[13] = {
 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};

void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
	list->moves[list->count].move = move;
	list->moves[list->count].score = 0;
	list->count ++;
}

void AddCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
	list->moves[list->count].move = move;
	list->moves[list->count].score = 0;
	list->count ++;
}

void AddEnPassantMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
	list->moves[list->count].move = move;
	list->moves[list->count].score = 0;
	list->count ++;
}

void AddWhitePawnCapMove( const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list ) {
	
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

void AddWhitePawnMove( const S_BOARD *pos, const int from, const int to, S_MOVELIST *list ) {

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

void AddBlackPawnCapMove( const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list ) {

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

void AddBlackPawnMove( const S_BOARD *pos, const int from, const int to, S_MOVELIST *list ) {

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

void GenerateAllMoves( const S_BOARD *pos, S_MOVELIST *list) {

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

	printf("\n\nSide: %d\n", side);

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

			if(sq+9 == pos->enPassantSQ) {
				AddCaptureMove(pos, MOVE(sq, (sq+9), EMPTY, EMPTY, MFLAGEP), list);
			}

			if(sq+11 == pos->enPassantSQ) {
				AddCaptureMove(pos, MOVE(sq, (sq+11), EMPTY, EMPTY, MFLAGEP), list);
			}

		}


	} else {
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

			if(sq-9 == pos->enPassantSQ) {
				AddCaptureMove(pos, MOVE(sq, (sq-9), EMPTY, EMPTY, MFLAGEP), list);
			}

			if(sq-11 == pos->enPassantSQ) {
				AddCaptureMove(pos, MOVE(sq, (sq-11), EMPTY, EMPTY, MFLAGEP), list);
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

			printf("Piece:%c on %s\n",PceChar[piece], printSquare(sq));

			//looping through all the direction the piece can go
			for(i = 0; i < NumDir[piece]; i++) {
				dir = PieceDir[piece][i];
				tmpSq = sq + dir;

				//going in that direction until the square isnt empty
				while(pos->pieces[tmpSq] == EMPTY) {
					printf("\t\tNormal on %s\n",printSquare(tmpSq));
					dir += PieceDir[piece][i];
					tmpSq = sq + dir;
				}

				if(!SQOFFBOARD(tmpSq) ) {
					if (PieceColor[pos->pieces[tmpSq]] == side ^ 1) {
						//Capture move
						printf("\t\tCapture on %s\n",printSquare(tmpSq));
					}
				}

			}

		}


		piece = LoopSlidePiece[pieceIndex++];
	}

	/*NonSliding pieces*/
	pieceIndex = LoopNonSlideIndex[side];
	piece = LoopNonSlidePiece[pieceIndex++];

	while(piece != 0) {
		ASSERT(PieceValid(piece));

		for(pieceNum = 0; pieceNum < pos->pceNum[piece]; pieceNum++) {
			sq = pos->pList[piece][pieceNum];
			ASSERT(SqOnBoard(sq));

			printf("Piece:%c on %s\n",PceChar[piece], printSquare(sq));

			for(i = 0; i < NumDir[piece]; i++ ) {
				dir = PieceDir[piece][i];
				tmpSq = sq + dir;

				if(!SQOFFBOARD(tmpSq)) {
					if(pos->pieces[tmpSq] == EMPTY ) {
						//quiet move
						printf("\t\tNormal on %s\n",printSquare(tmpSq));
					} else if (PieceColor[pos->pieces[tmpSq]] == side ^ 1) {
						//Capture move
						printf("\t\tCapture on %s\n",printSquare(tmpSq));
					}
				}

			}
		}
		piece = LoopNonSlidePiece[pieceIndex++];
	}
}



