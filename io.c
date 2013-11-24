#include "stdio.h"
#include "defs.h"

char *printSquare(const int sq) {
	static char SqStr[3];

	int file = GetFile[sq];
	int rank = GetRank[sq];

	sprintf(SqStr, "%c%c", ('a'+file), ('1'+rank));

	return SqStr;
}

char *printMove(const int move) {
	static char MoveStr[6];
	
	int fileFrom = GetFile[FROM(move)];
	int rankFrom = GetRank[FROM(move)];
	int fileTo = GetFile[TO(move)];
	int rankTo = GetRank[TO(move)];

	int promoted = PROMOTED(move);

	if (promoted) {
		char promotedChar;

		if(IsRQ(promoted) && IsBQ(promoted)) 
			promotedChar = 'q';

		else if(IsBQ(promoted)) 
			promotedChar = 'b';

		else if(IsRQ(promoted))	
			promotedChar = 'r';

		else 
			promotedChar = 'n';


		sprintf(MoveStr, "%c%c%c%c%c", ('a'+fileFrom), ('1'+rankFrom), ('a'+fileTo), ('1'+rankTo), promotedChar );
	}
	else {
		sprintf(MoveStr,"%c%c%c%c", ('a'+fileFrom), ('1'+rankFrom), ('a'+fileTo), ('1'+rankTo) );
	}

	return MoveStr;
}

int ParseMove(char *ptrChar, S_BOARD *pos) {
	if(ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE;
    if(ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;
    if(ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;
    if(ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;

    int from = FRtoSQ(ptrChar[0] - 'a', ptrChar[1] - '1');
    int to = FRtoSQ(ptrChar[2] - 'a', ptrChar[3] - '1');	

    ASSERT(SqOnBoard(from) && SqOnBoard(to));

    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);      
    int MoveNum = 0;
	int Move = 0;
	int PromPce = EMPTY;

	for(MoveNum = 0; MoveNum < list->count; MoveNum++) {	
		Move = list->moves[MoveNum].move;
		if(FROM(Move)==from && TO(Move)==to) {
			PromPce = PROMOTED(Move);
			if(PromPce!=EMPTY) {
				if(IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4]=='q') {
					return Move;
				} else if(IsRQ(PromPce) && ptrChar[4]=='r') {
					return Move;
				} else if(IsBQ(PromPce) && ptrChar[4]=='b') {
					return Move;
				} else if(IsKn(PromPce)&& ptrChar[4]=='n') {
					return Move;
				}
				continue;
			}
			return Move;
		}
    }
	
    return NOMOVE;	
}

void PrintMoveList(const S_MOVELIST *list) {
	int index = 0;
	int score = 0;
	int move = 0;
	printf("MoveList:\n",list->count);
	
	for(index = 0; index < list->count; ++index) {
	
		move = list->moves[index].move;
		score = list->moves[index].score;
		
		printf("Move:%d > %s (score:%d)\n",index+1,printMove(move),score);
	}
	printf("MoveList Total %d Moves:\n\n",list->count);
}