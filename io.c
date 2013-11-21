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