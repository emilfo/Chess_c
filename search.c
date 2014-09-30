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
	int bestMove = NOMOVE;
	int bestScore = -infinite;
	int currentDepth;
	int pvNum;
	int pvMoves = 0;

	ClearForSearch(pos, info);

	for (currentDepth = 1; currentDepth <= info->depth; currentDepth++) {
		bestScore = AlphaBeta(-infinite, infinite, currentDepth, pos, info,
																		TRUE);
		//TODO out of time?
		pvMoves = GetPvLine(currentDepth, pos);
		bestMove = pos->PvArray[0];

		printf("Depth:%d, score:%d, move:%s, nodes:%ld ", currentDepth,
								bestScore, printMove(bestMove), info->nodes);
		printf("pv");
		for (pvNum = 0; pvNum < pvMoves; pvNum++) {
			printf(" %s", printMove(pos->PvArray[pvNum]));
		}
		printf("\n");
}
