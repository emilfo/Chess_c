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

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {
	int i, j;

	for (i = 0; i < 13; i++) {
		for (j = 0; j < BRD_SQ_NUM; j++) {
			pos->searchHistory[i][j] = 0;
		}
	}

	for (i = 0; i < 2; i++) {
		for (j = 0; j < MAXDEPTH; j++) {
			pos->searchKillers[i][j] = 0;
		}
	}

	ClearPvTable(pos->PvTable);
	pos->ply = 0;

	info->starttime = GetTimeMs();
	info->stopped = 0;
	info->nodes = 0;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos,
											S_SEARCHINFO *info, int DoNull) {
	return 0;//TODO
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
	return 0;//TODO
}

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
	//TODO iterative deepening, search init
}
