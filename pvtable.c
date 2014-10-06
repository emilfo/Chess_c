#include "stdio.h"
#include "defs.h"
#include <inttypes.h>


int GetPvLine(const int depth, S_BOARD *pos) {
	ASSERT(depth < MAXDEPTH);

	int move = ProbePvMove(pos);
	int count = 0;

	while(move != NOMOVE && count < depth) {
		ASSERT(count < MAXDEPTH);

		if(MoveExists(pos, move)) {
			MakeMove(pos, move);
			pos->PvArray[count++] = move;
		} else {
			break;
		}
		move = ProbePvMove(pos);
	}

	//Taking back all moves that GetPvLine did
	while(pos->ply > 0) {
		TakeMove(pos);
	}

	return count;
}

void ClearHashTable(S_HASHTABLE *table) {
	int i;
	S_HASHENTRY *tableEntry = table->pTable;

	for (i = 0; i < table->numEntries; i++) {
		tableEntry->posKey = ZERO64;
		tableEntry->move = NOMOVE;
		tableEntry->depth = 0;
		tableEntry->score = 0;
		tableEntry->flag = 0;
		tableEntry++;
	}
	table->newWrite = 0;
}

void InitHashTable(S_HASHTABLE *table, int size) {
    table->numEntries = size / sizeof(S_HASHENTRY);
    //table->numEntries -= 2;

    if (table->pTable) free(table->pTable);

    table->pTable = (S_HASHENTRY *) malloc(table->numEntries * sizeof(S_HASHENTRY));
	if (table->pTable == NULL) {
		printf("malloc with size:%d(MB) failed, trying half\n", size/100000);
		InitHashTable(table, size/2);
		return;
	}
    ClearHashTable(table);
    printf("HashTable init complete with %d entries\n",table->numEntries);
}

int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth) {
	int index = pos->posKey % pos->HashTable->numEntries;

	ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1);
    ASSERT(depth>=1&&depth<MAXDEPTH);
    ASSERT(alpha<beta);
    ASSERT(alpha>=-INFINITE&&alpha<=INFINITE);
    ASSERT(beta>=-INFINITE&&beta<=INFINITE);
    ASSERT(pos->ply>=0&&pos->ply<MAXDEPTH);

	if (pos->HashTable->pTable[index].posKey == pos->posKey) {
		//printf("poskey %llu\n", pos->posKey);
		*move = pos->HashTable->pTable[index].move;

		if (pos->HashTable->pTable[index].depth >= depth) {
			pos->HashTable->hit++;

			ASSERT(pos->HashTable->pTable[index].depth >= 1
					&& pos->HashTable->pTable[index].depth<MAXDEPTH);
			ASSERT(pos->HashTable->pTable[index].flag >= HFALPHA
					&& pos->HashTable->pTable[index].flag <= HFEXACT);

			*score = pos->HashTable->pTable[index].score;
			//reset the mate score
			if (*score > ISMATE) {
				*score += pos->ply;
			} else if (*score < -ISMATE) {
				*score -= pos->ply;
			}

			if (!(*score >= -INFINITE) && !(*score <= INFINITE)) {
				printf("poskey:%"PRIu64", move%d, score%d, depth%d, flag%d\n", pos->HashTable->pTable[index].posKey, pos->HashTable->pTable[index].move, pos->HashTable->pTable[index].score, pos->HashTable->pTable[index].depth, pos->HashTable->pTable[index].flag);
			}
			//printf("score %d\n", *score);
			ASSERT(*score>=-INFINITE && *score<=INFINITE);
			ASSERT(alpha>=-INFINITE && alpha<=INFINITE);
			ASSERT(beta>=-INFINITE && beta<=INFINITE);
			switch (pos->HashTable->pTable[index].flag) {
				case HFALPHA:
				if(*score > -50000) {
					if(alpha > -50000) {
						if(*score <= alpha) {
							*score = alpha;
							return TRUE;
						}
					}
				}
				break;
				case HFBETA:
				if(*score >= beta) {
					*score = beta;
					return TRUE;
				}
				break;
				case HFEXACT:
					return TRUE;
					break;
				default:
					ASSERT(FALSE);
					break;
			}
		}
	}
	return FALSE;
}





void StoreHashEntry(S_BOARD *pos, const int move, int score,
		const int flags, const int depth) {

	int index = pos->posKey % pos->HashTable->numEntries;


	if( pos->HashTable->pTable[index].posKey == 0 ) {
		pos->HashTable->newWrite++;
	} else {
		pos->HashTable->overWrite++;
	}

	if (score > ISMATE) {
		score -= pos->ply;
	} else if (score < -ISMATE) {
		score += pos->ply;
	}
	ASSERT(index >= 0 && index < pos->HashTable->numEntries);
	ASSERT(depth>=1 && depth<MAXDEPTH);
    ASSERT(flags>=HFALPHA && flags<=HFEXACT);
    ASSERT(pos->ply >= 0 && pos->ply<MAXDEPTH);
    ASSERT(score>=-INFINITE && score<=INFINITE);

	pos->HashTable->pTable[index].posKey = pos->posKey;
	pos->HashTable->pTable[index].move = move;
	pos->HashTable->pTable[index].score = score;
	pos->HashTable->pTable[index].depth = depth;
	pos->HashTable->pTable[index].flag = flags;
}

int ProbePvMove(const S_BOARD *pos) {
	int index = pos->posKey % pos->HashTable->numEntries;
	ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1);

	if (pos->HashTable->pTable[index].posKey == pos->posKey) {
		return pos->HashTable->pTable[index].move;
	}

	return NOMOVE;
}

