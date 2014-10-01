#include "stdio.h"
#include "defs.h"

#define INFINITE 30000
#define MATE 29000

static void CheckUp(S_SEARCHINFO *info) {
	if(info->timeset == TRUE && GetTimeMs() > info->stoptime) {
		info->stopped = TRUE;
	}
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

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
	ASSERT(CheckBoard(pos));

	//Checks if time is up for every 2048 node we check
	if((info->nodes & 2047) == 0) {
		CheckUp(info);
	}

	info->nodes++;

	if (IsRepetition(pos) || pos->fiftyMove >= 100) {
		return 0;
	}

	if (pos->ply > MAXDEPTH - 1) {
		return EvalPosition(pos);
	}

	int Score = EvalPosition(pos);

	if (Score >= beta) {
		return beta;
	}
	if (Score > alpha) {
		alpha = Score;
	}

	S_MOVELIST list[1];
	GenerateAllCaps(pos,list);

	int MoveNum = 0;
	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	Score = -INFINITE;
	int PvMove = ProbePvTable(pos);

	for (MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		PickNextMove(MoveNum, list);

		if (!MakeMove(pos,list->moves[MoveNum].move))  {
			continue;
		}

		Legal++;
		Score = -Quiescence(-beta, -alpha, pos, info);
		TakeMove(pos);

		if (info->stopped == TRUE) {
			return 0;
		}

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

	if (alpha != OldAlpha) {
		StorePvMove(pos, BestMove);
	}

	return alpha;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos,
											S_SEARCHINFO *info, int DoNull) {
	ASSERT(checkBoard(pos));

	if(depth == 0) {
		info->nodes++;
		return Quiescence(alpha, beta, pos, info);
		//return EvalPosition(pos);
	}

	//Checks if time is up for every 2048 node we check
	if((info->nodes & 2047) == 0) {
		CheckUp(info);
	}

	info->nodes++;

	if((IsRepetition(pos) || pos->fiftyMove >= 100) && pos->ply) {
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
	int PvMove = ProbePvTable(pos);

	//If we ahve a PV-move, we run through the list of moves and score it best
	if (PvMove !=NOMOVE) {
		for (MoveNum = 0; MoveNum < list->count; MoveNum++) {
			if (list->moves[MoveNum].move == PvMove) {
				list->moves[MoveNum].score = 2000000;
				break;
			}
		}
	}

	for (MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		PickNextMove(MoveNum, list);

		if (!MakeMove(pos,list->moves[MoveNum].move))  {
			continue;
		}

		Legal++;
		Score = -AlphaBeta(-beta, -alpha, depth-1, pos, info, TRUE);
		TakeMove(pos);

		if (info->stopped == TRUE) {
			return 0;
		}

		if (Score > alpha) {
			if (Score >= beta) {
				if(Legal==1) {
					info->fhf++;
				}
				info->fh++;

				//if its not a capture that cuts off a beta, its a killer move
				if(!(list->moves[MoveNum].move & MFLAGCAP)) {
					pos->searchKillers[1][pos->ply] =
												pos->searchKillers[0][pos->ply];
					pos->searchKillers[0][pos->ply] = list->moves[MoveNum].move;
				}
				return beta;
			}
			alpha = Score;
			BestMove = list->moves[MoveNum].move;

			if(!(list->moves[MoveNum].move & MFLAGCAP)) {
				pos->searchHistory[pos->pieces[FROM(BestMove)]][TO(BestMove)] += depth;
			}
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

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
	int bestMove = NOMOVE;
	int bestScore = -INFINITE;
	int currentDepth;
	int pvNum;
	int pvMoves = 0;

	ClearForSearch(pos, info);

	//iterative deepening, calling alphabeta for every depth
	for (currentDepth = 1; currentDepth <= info->depth; currentDepth++) {
		bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, pos, info,
																		TRUE);
		if(info->stopped == TRUE) {
			break;
		}

		pvMoves = GetPvLine(currentDepth, pos);
		bestMove = pos->PvArray[0];

		printf("info score cp %d depth %d nodes %ld time %d ",
				bestScore, currentDepth, info->nodes, GetTimeMs() - info->starttime);
		printf("pv");
		for (pvNum = 0; pvNum < pvMoves; pvNum++) {
			printf(" %s", printMove(pos->PvArray[pvNum]));
		}
		printf("\n");
		printf("Ordering:%.2f\n",(info->fhf/info->fh));
	}
	printf("bestmove %s\n", printMove(bestMove));
}
