#include "stdio.h"
#include "defs.h"

#define INFINITE 30000
#define MATE 29000

static void CheckUp() {
	//TODO check if time is up or interrupted
}

static void PickNextMove(int moveNum, S_MOVELIST *list) {
	S_MOVE temp;
	int i;
	int bestScore = -INFINITE;
	int bestNum = moveNum;

	for(i = moveNum; i < list->count; i++) {
		if(list->moves[i].score > bestScore) {
			bestScore = list->moves[i].score;
			bestNum = i;
		}
	}
	temp = list->moves[moveNum];
	list->moves[moveNum] = list->moves[bestNum];
	list->moves[bestNum] = temp;
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
	info->fh = 0;
	info->fhf = 0;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos,
											S_SEARCHINFO *info, int DoNull) {
	ASSERT(checkBoard(pos));

	if(depth == 0) {
		info->nodes++;
		return EvalPosition(pos);
	}

	info->nodes++;

	if(IsRepetition(pos) || pos->fiftyMove >= 100) {
		return 0;
	}

	if(pos->ply > MAXDEPTH - 1) {
		return EvalPosition(pos);
	}

	S_MOVELIST list[1];
	GenerateAllMoves(pos,list);

	int MoveNum = 0;
	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	int Score = -INFINITE;

	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		PickNextMove(MoveNum, list);

		if (!MakeMove(pos,list->moves[MoveNum].move))  {
			continue;
		}

		Legal++;
		Score = -AlphaBeta(-beta, -alpha, depth-1, pos, info, TRUE);
		TakeMove(pos);

		if (Score > alpha) {
			if (Score >= beta) {
				if(Legal==1) {
					info->fhf++;
				}
				info->fh++;
				return beta;
			}
			alpha = Score;
			BestMove = list->moves[MoveNum].move;
		}
	}

	if (Legal == 0) {
		if (SqAttacked(pos->kingSQ[pos->side],pos->side^1,pos)) {
			return -MATE + pos->ply;
		} else {
			return 0;
		}
	}

	if (alpha != OldAlpha) {
		StorePvMove(pos, BestMove);
	}

	return alpha;
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
	return 0;//TODO
}

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
	int bestMove = NOMOVE;
	int bestScore = -INFINITE;
	int currentDepth;
	int pvNum;
	int pvMoves = 0;

	ClearForSearch(pos, info);

	for (currentDepth = 1; currentDepth <= info->depth; currentDepth++) {
		bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, pos, info,
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
		printf("Ordering:%.2f\n",(info->fhf/info->fh));
	}
}
