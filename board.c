#include "stdio.h"
#include "defs.h"

//Debug method to check if listmaterial is updated correctly
int checkBoard(const S_BOARD *pos) {   
 
	int t_pceNum[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int t_bigPce[2] = { 0, 0};
	int t_majPce[2] = { 0, 0};
	int t_minPce[2] = { 0, 0};
	int t_material[2] = { 0, 0};
	
	int sq64,t_piece,t_pce_num,sq120,color,pcount;
	
	U64 t_pawns[3] = {ZERO64, ZERO64, ZERO64};
	
	t_pawns[WHITE] = pos->pawns[WHITE];
	t_pawns[BLACK] = pos->pawns[BLACK];
	t_pawns[BOTH] = pos->pawns[BOTH];
	
	// check piece lists
	for(t_piece = wP; t_piece <= bK; t_piece++) {
		for(t_pce_num = 0; t_pce_num < pos->pceNum[t_piece]; t_pce_num++) {
			sq120 = pos->pList[t_piece][t_pce_num];
			ASSERT(pos->pieces[sq120]==t_piece);
		}	
	}
			
	// check piece count and other counters	
	for(sq64 = 0; sq64 < 64; ++sq64) {
		sq120 = SQ120(sq64);
		t_piece = pos->pieces[sq120];
		t_pceNum[t_piece]++;
		color = PieceColor[t_piece];
		if( PieceBig[t_piece] == TRUE) t_bigPce[color]++;
		if( PieceMin[t_piece] == TRUE) t_minPce[color]++;
		if( PieceMaj[t_piece] == TRUE) t_majPce[color]++;
		
		t_material[color] += PieceVal[t_piece];
	}
		
	for(t_piece = wP; t_piece <= bK; ++t_piece) {
		ASSERT(t_pceNum[t_piece]==pos->pceNum[t_piece]);	
	}


	// check bitboards count
	pcount = CNT(t_pawns[WHITE]);
	ASSERT(pcount == pos->pceNum[wP]);
	pcount = CNT(t_pawns[BLACK]);
	ASSERT(pcount == pos->pceNum[bP]);
	pcount = CNT(t_pawns[BOTH]);
	ASSERT(pcount == (pos->pceNum[bP] + pos->pceNum[wP]));
		
	// check bitboards squares
	while(t_pawns[WHITE]) {
		sq64 = POP(&t_pawns[WHITE]);
		ASSERT(pos->pieces[SQ120(sq64)] == wP);
	}
	
	while(t_pawns[BLACK]) {
		sq64 = POP(&t_pawns[BLACK]);
		ASSERT(pos->pieces[SQ120(sq64)] == bP);
	}
	
	while(t_pawns[BOTH]) {
		sq64 = POP(&t_pawns[BOTH]);
		ASSERT( (pos->pieces[SQ120(sq64)] == bP) || (pos->pieces[SQ120(sq64)] == wP) );
	}

	ASSERT(t_material[WHITE]==pos->material[WHITE] && t_material[BLACK]==pos->material[BLACK]);
	ASSERT(t_minPce[WHITE]==pos->minPce[WHITE] && t_minPce[BLACK]==pos->minPce[BLACK]);
	ASSERT(t_majPce[WHITE]==pos->majPce[WHITE] && t_majPce[BLACK]==pos->majPce[BLACK]);
	ASSERT(t_bigPce[WHITE]==pos->bigPce[WHITE] && t_bigPce[BLACK]==pos->bigPce[BLACK]);	
	
	ASSERT(pos->side==WHITE || pos->side==BLACK);
	ASSERT(GeneratePosKey(pos)==pos->posKey);
	
	ASSERT(pos->enPassantSQ==NO_SQ || ( GetRank[pos->enPassantSQ]==RANK_6 && pos->side == WHITE)
		 || ( GetRank[pos->enPassantSQ]==RANK_3 && pos->side == BLACK));
	
	ASSERT(pos->pieces[pos->kingSQ[WHITE]] == wK);
	ASSERT(pos->pieces[pos->kingSQ[BLACK]] == bK);
		 
	return TRUE;	
}

void UpdateListsMaterial (S_BOARD *pos) {
	int piece;
	int sq;
	int color;

	pos->material[WHITE] = pos->material[BLACK] = 0;

	

	for(sq = 0; sq < BRD_SQ_NUM; sq++) {
		piece = pos->pieces[sq];

		if(piece != OFFBOARD && piece != EMPTY) {
			color = PieceColor[piece];
			if(PieceMaj[piece]) {
				pos->majPce[color]++;
				pos->bigPce[color]++;
				if(piece == wK || piece == bK) {
					pos->kingSQ[color] = sq;
				}
			}
			else if(PieceMin[piece]) {
				pos->minPce[color]++;
				pos->bigPce[color]++;
			}

			else if(piece == wP || piece == bP) {
				SETBIT(pos->pawns[color], SQ64(sq));
				SETBIT(pos->pawns[BOTH], SQ64(sq));
			}

			//adding the pieceavalue to color
			pos->material[color] += PieceVal[piece];
			//adding the piece SQ to the piecelist
			pos->pList[piece][ pos->pceNum[piece] ++] = sq; 
			//pceNum is the number of the same pieces already added
		}
	}
}

int parseFEN(char *fen, S_BOARD *pos) {
	ASSERT(fen != NULL);
	ASSERT(pos != NULL);

	//starts with resetting the board
	resetBoard(pos);

	int  rank = RANK_8;
    int  file = FILE_A;
    int  piece = 0;
    int  count = 0;
    int  i = 0; 
	int  sq64 = 0; 
	int  sq120 = 0;

	//looping through the positions of the board
	while ((rank >= RANK_1) && *fen) {
	    count = 1;
		switch (*fen) {
            case 'p': piece = bP; break;
            case 'r': piece = bR; break;
            case 'n': piece = bN; break;
            case 'b': piece = bB; break;
            case 'k': piece = bK; break;
            case 'q': piece = bQ; break;
            case 'P': piece = wP; break;
            case 'R': piece = wR; break;
            case 'N': piece = wN; break;
            case 'B': piece = wB; break;
            case 'K': piece = wK; break;
            case 'Q': piece = wQ; break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                count = *fen - '0';
                break;

            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                fen++;
                continue;              

            default:
                printf("FEN error \n");
                return -1;
        }		
		
		for (i = 0; i < count; i++) {			
            sq64 = rank * 8 + file;
			sq120 = SQ120(sq64);
            if (piece != EMPTY) {
                pos->pieces[sq120] = piece;
            }
			file++;
        }
		fen++;
	}

	//debugging added to be certain the FEN board is correctly traversed
	ASSERT (*fen == 'w' || *fen == 'b')

	pos->side = (*fen == 'w')? WHITE : BLACK;
	fen ++; fen++;

	while (*fen != ' ') {
		switch (*fen) {
			case 'K': pos->castlePerm |= WKCA; break;
			case 'Q': pos->castlePerm |= WQCA; break;
			case 'k': pos->castlePerm |= BKCA; break;
			case 'q': pos->castlePerm |= BQCA; break;
			default:	break;
		}
		fen++;
	}
	fen++;

	ASSERT(pos->castlePerm >= 0 && pos->castlePerm <=15);

	if(*fen != '-') {
		file = fen[0] - 'a';
		rank = fen[1] - '1';

		//checking to see if the castling is done right and is inside the board
		ASSERT(pos->side == WHITE || (rank == RANK_6 && (file >= FILE_A && file >= FILE_H)));
		ASSERT(pos->side == BLACK || (rank == RANK_3 && (file >= FILE_A && file >= FILE_H)));

		pos->enPassantSQ = FRtoSQ(file, rank);
	}

	pos->posKey = GeneratePosKey(pos);

	UpdateListsMaterial(pos);
	return 0;
}


//resets the board to startposition
void resetBoard (S_BOARD *pos) {
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
	for(i = 0; i < 2; i++) {
		pos->bigPce[i] = 0;
		pos->majPce[i] = 0;
		pos->minPce[i] = 0;
	}
	for(i = 0; i < 3; i++) {
		pos->pawns[i] = ZERO64;
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
	pos->posKey = ZERO64;
}

void printBoard (const S_BOARD *pos) {
	int sq = 0;
	int file = 0;
	int rank = 0;
	int piece = 0;

	printf("\nBoard:\n\n");

	for(rank = RANK_8; rank >= RANK_1; rank--) {
		printf("%d ", rank+1);
		for(file = FILE_A; file <= FILE_H; file++) {
			sq = FRtoSQ(file, rank);
			piece = pos->pieces[sq];
			printf("%2c", PceChar[piece]);
		}
		printf("\n");
	}

	printf("\n  ");
	for(file = FILE_A; file <= FILE_H; file++) {
		printf("%2c", 'A'+file);
	}

	printf("\nside: %c", SideChar[pos->side]);
	printf("\nenPassant-Square: %d", pos->enPassantSQ);

	printf("\ncastle:%c%c%c%c\n",
			pos->castlePerm & WKCA ? 'K' : '-',
			pos->castlePerm & WQCA ? 'Q' : '-',
			pos->castlePerm & BKCA ? 'k' : '-',
			pos->castlePerm & BQCA ? 'q' : '-');

	printf("posKey: %11X\n", pos ->posKey);
}