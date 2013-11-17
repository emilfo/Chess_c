#include "stdio.h"
#include "defs.h"

int parse_FEN(char *fen, S_BOARD *pos) {
	ASSERT(fen != NULL);
	ASSERT(pos != NULL);

	//starts with resetting the board
	ResetBoard(pos);

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
	return 0;
}


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

