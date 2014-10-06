#include "stdio.h"
#include "defs.h"

#define INFINITE 30000
#define MATE 29000

static void CheckUp(S_SEARCHINFO *info) {
	if(info->timeset == TRUE && GetTimeMs() > info->stoptime) {
		info->stopped = TRUE;
	}

	ReadInput(info);
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

void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {
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

	pos->HashTable->overWrite = 0;
	pos->HashTable->hit = 0;
	pos->HashTable->cut = 0;
	pos->ply = 0;

	info->stopped = FALSE;
	info->nodes = 0;
	info->fh = 0;
	info->fhf = 0;
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
	ASSERT(checkBoard(pos));

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
	//int OldAlpha = alpha;
	//int BestMove = NOMOVE;
	Score = -INFINITE;
	//int PvMove = ProbePvTable(pos);

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
			//BestMove = list->moves[MoveNum].move;
		}
	}
/*
	if (alpha != OldAlpha) {
		//StorePvMove(pos, BestMove);
	}*/

	return alpha;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull) {
	ASSERT(checkBoard(pos));
	if(depth == 0) {
		info->nodes++;
		int tmpScore = Quiescence(alpha, beta, pos, info);
		ASSERT(tmpScore>=-INFINITE && tmpScore<=INFINITE);
		return tmpScore;
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
		int tmpScore = EvalPosition(pos);
		ASSERT(tmpScore>=-INFINITE && tmpScore<=INFINITE);
		return tmpScore;
	}

	int InCheck = SqAttacked(pos->kingSQ[pos->side], pos->side^1, pos);
	if(InCheck == TRUE) {
		depth++;
	}

	int Score = -30000;
	int PvMove = NOMOVE;
/*
	if( ProbeHashEntry(pos, &PvMove, &Score, alpha, beta, depth) == TRUE ) {
		pos->HashTable->cut++;
		ASSERT(Score>=-INFINITE && Score<=INFINITE);
		return Score;
	}
*/
	if( DoNull && !InCheck && pos->ply && (pos->bigPce[pos->side] > 0) && depth >= 4) {
		MakeNullMove(pos);
		Score = -AlphaBeta(-beta, -beta + 1, depth-4, pos, info, FALSE);
		TakeNullMove(pos);

		if (info->stopped == TRUE) {
			return 0;
		}

		if(Score >= beta && abs(Score) < ISMATE) {
			ASSERT(beta>=-INFINITE && beta<=INFINITE);
			return beta;
		}
	}

	S_MOVELIST list[1];
	GenerateAllMoves(pos,list);

	int MoveNum = 0;
	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	int bestScore = -INFINITE;
	Score = -INFINITE;

	//If we ahve a PV-move, we run through the list of moves and score it best
	if (PvMove !=NOMOVE) {
		for (MoveNum = 0; MoveNum < list->count; MoveNum++) {
			if (list->moves[MoveNum].move == PvMove) {
				list->moves[MoveNum].score = 2000000;
				break;
			}
		}
	}

	for (MoveNum = 0; MoveNum < list->count; MoveNum++) {
		PickNextMove(MoveNum, list);

		if (!MakeMove(pos,list->moves[MoveNum].move))  {
			continue;
		}

		Legal++;
		ASSERT(beta <= INFINITE);
		ASSERT(alpha <= INFINITE);
		ASSERT(depth >= 1);
		ASSERT(checkBoard(pos));
		ASSERT(info->nodes >= 0);
		int tmpScore = -AlphaBeta(-beta, -alpha, depth-1, pos, info, TRUE);
		Score = tmpScore;
		TakeMove(pos);

		if (info->stopped == TRUE) {
			return 0;
		}

		if( Score > bestScore ) {
			bestScore = Score;
			BestMove = list->moves[MoveNum].move;

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

					StoreHashEntry(pos, BestMove, beta, HFBETA, depth);

					ASSERT(beta>=-INFINITE && beta<=INFINITE);
					return beta;
				}
				alpha = Score;

				if(!(list->moves[MoveNum].move & MFLAGCAP)) {
					pos->searchHistory[pos->pieces[FROM(BestMove)]][TO(BestMove)] += depth;
				}
			}
		}
	}

	if (Legal == 0) {
		if (InCheck == TRUE) {
			ASSERT((-INFINITE + pos->ply) >=-INFINITE && (-INFINITE + pos->ply)<=INFINITE);
			return -INFINITE + pos->ply;
		} else {
			return 0;
		}
	}

	if (alpha != OldAlpha) {
		StoreHashEntry(pos, BestMove, bestScore, HFEXACT, depth);
		//StorePvMove(pos, BestMove);
	} else {
		StoreHashEntry(pos, BestMove, alpha, HFALPHA, depth);
	}

	ASSERT(alpha>=-INFINITE && alpha<=INFINITE);
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
		bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, pos, info, TRUE);

		if(info->stopped == TRUE) {
			break;
		}

		pvMoves = GetPvLine(currentDepth, pos);
		bestMove = pos->PvArray[0];

		if (info->GAME_MODE == UCIMODE) {
			printf("info score cp %d depth %d nodes %ld time %ld ",
				bestScore, currentDepth, info->nodes, (GetTimeMs() - info->starttime));
		}
		else if (info->GAME_MODE == XBOARDMODE && info->POST_THINKING == TRUE) {
			printf("%d %d %ld %ld ",
					currentDepth, bestScore, (GetTimeMs()-info->starttime)/10,
					info->nodes);
		}
		else if (info->POST_THINKING == TRUE) {
			printf ("score:%d depth:%d nodes:%ld time:%ld(ms) ",
					bestScore, currentDepth, info->nodes, GetTimeMs()-info->starttime);
		}

		if (info->GAME_MODE == UCIMODE || info->POST_THINKING == TRUE) {
			pvMoves = GetPvLine(currentDepth, pos);
			printf("pv");
			for (pvNum = 0; pvNum < pvMoves; pvNum++) {
				printf(" %s", printMove(pos->PvArray[pvNum]));
			}
			printf("\n");
		}
	}

	if (info->GAME_MODE == UCIMODE) {
		printf("bestmove %s\n", printMove(bestMove));
	} else if (info->GAME_MODE == XBOARDMODE) {
		printf("move %s\n", printMove(bestMove));
		MakeMove(pos, bestMove);
	} else {
		printf("\n\n ===== ELOmif makes move %s =====\n\n", printMove(bestMove));
		MakeMove(pos, bestMove);
		printBoard(pos);
	}
}

void MirrorBoard(S_BOARD *pos) {

    int tempPiecesArray[64];
    int tempSide = pos->side^1;
	int SwapPiece[13] = { EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK };
    int tempCastlePerm = 0;
    int tempEnPas = NO_SQ;

	int sq;
	int tp;

    if (pos->castlePerm & WKCA) tempCastlePerm |= BKCA;
    if (pos->castlePerm & WQCA) tempCastlePerm |= BQCA;

    if (pos->castlePerm & BKCA) tempCastlePerm |= WKCA;
    if (pos->castlePerm & BQCA) tempCastlePerm |= WQCA;

	if (pos->enPassantSQ != NO_SQ)  {
        tempEnPas = SQ120(Mirror64[SQ64(pos->enPassantSQ)]);
    }

    for (sq = 0; sq < 64; sq++) {
        tempPiecesArray[sq] = pos->pieces[SQ120(Mirror64[sq])];
    }

    resetBoard(pos);

	for (sq = 0; sq < 64; sq++) {
        tp = SwapPiece[tempPiecesArray[sq]];
        pos->pieces[SQ120(sq)] = tp;
    }

	pos->side = tempSide;
    pos->castlePerm = tempCastlePerm;
    pos->enPassantSQ = tempEnPas;

    pos->posKey = GeneratePosKey(pos);

	UpdateListsMaterial(pos);

    ASSERT(checkBoard(pos));
}
