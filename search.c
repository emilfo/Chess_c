#include "stdio.h"
#include "defs.h"

static void CheckUp() {
	//TODO check if time is up or interrupted
}

static int IsRepetition(const S_BOARD *pos) {
	int index = 0;

	for(index = pos->hisPly - pos->fiftyMove; index < pos->hisPly-1; index++) {
		if(pos->posKey == pos->history[index].posKey) {
			return TRUE;
		}
	}
	
	return FALSE;
}

static void ClearForSearch(S_BOARD *pos, S_SEARCHINGO *info) {//TODO
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos,
											S_SEARCHINGO *info, int DoNull) {
	return 0;//TODO
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
	return 0;//TODO
}

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
	//TODO iterative deepening, search init
}
